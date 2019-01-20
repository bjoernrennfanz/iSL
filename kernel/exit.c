#include <pthread.h>
#include <user-signal.h>
#include "kernel/calls.h"
#include "kernel/futex.h"
#include "fs/fd.h"

static void halt_system(int status);

static bool exit_tgroup(struct task *task) {
    struct tgroup *group = task->group;
    lock(&group->lock);
    list_remove(&task->group_links);
    bool group_dead = list_empty(&group->threads);
    if (group_dead) {
        if (group->timer) {
            timer_set(group->timer, (struct timer_spec) {}, NULL);
        }
        unlock(&group->lock);
        lock(&pids_lock);
        list_remove(&group->pgroup);
        list_remove(&group->session);
        unlock(&pids_lock);
    } else {
        unlock(&group->lock);
    }
    return group_dead;
}

noreturn void do_exit(int status) {
    // has to happen before mem_release
    addr_t clear_tid = current->clear_tid;
    if (clear_tid) {
        pid_t_ zero = 0;
        if (user_put(clear_tid, zero) == 0)
            futex_wake(clear_tid, 1);
    }

    // release all our resources
    mem_release(current->mem);
    fdtable_release(current->files);
    fs_info_release(current->fs);
    sighand_release(current->sighand);
    bool group_dead = exit_tgroup(current);
    vfork_notify(current);

    // save things that our parent might be interested in
    current->exit_code = status;
    struct rusage_ rusage = rusage_get_current();
    lock(&current->group->lock);
    rusage_add(&current->group->rusage, &rusage);
    unlock(&current->group->lock);

    // the actual freeing needs pids_lock
    lock(&pids_lock);
    struct task *leader = current->group->leader;

    if (group_dead) {
        // reparent children
        struct task *new_parent = pid_get_task(1);
        struct task *child;
        list_for_each_entry(&current->children, child, siblings) {
            child->parent = new_parent;
        }

        // notify parent that we died
        struct task *parent = leader->parent;
        if (parent == NULL) {
            // init died
            halt_system(status);
        } else {
            lock(&parent->group->lock);
            leader->zombie = true;
            notify(&parent->group->child_exit);
            unlock(&parent->group->lock);
            send_signal(parent, leader->exit_signal);
        }
    }

    if (current != leader)
        task_destroy(current);
    unlock(&pids_lock);

    pthread_exit(NULL);
}

noreturn void do_exit_group(int status) {
    struct tgroup *group = current->group;
    lock(&group->lock);
    if (!group->doing_group_exit) {
        group->doing_group_exit = true;
        group->group_exit_code = status;
    } else {
        status = group->group_exit_code;
    }

    // kill everyone else in the group
    struct task *task;
    list_for_each_entry(&group->threads, task, group_links) {
        send_signal(task, SIGKILL_);
    }

    unlock(&group->lock);
    do_exit(status);
}

void (*exit_hook)(int code) = NULL;
// always called from init process
static void halt_system(int status) {
    // brutally murder everything
    // which will leave everything in an inconsistent state. I will solve this problem later.
    for (int i = 2; i < MAX_PID; i++) {
        struct task *task = pid_get_task(i);
        if (task != NULL)
            pthread_kill(task->thread, SIGKILL);
    }

    // unmount all filesystems
    lock(&mounts_lock);
    struct mount *mount, *tmp;
    list_for_each_entry_safe(&mounts, mount, tmp, mounts) {
        mount_remove(mount);
    }
    unlock(&mounts_lock);

    if (exit_hook != NULL)
        exit_hook(status);
}

dword_t sys_exit(dword_t status) {
    STRACE("exit(%d)", status);
    do_exit(status << 8);
}

dword_t sys_exit_group(dword_t status) {
    STRACE("exit_group(%d)", status);
    do_exit_group(status << 8);
}

// returns 0 if the task cannot be reaped, returns 1 if the task was reaped
static int reap_if_zombie(struct task *task, addr_t status_addr, addr_t rusage_addr) {
    assert(task_is_leader(task));
    if (!task->zombie)
        return 0;
    lock(&task->group->lock);

    dword_t exit_code = task->exit_code;
    if (task->group->doing_group_exit)
        exit_code = task->group->group_exit_code;
    if (status_addr != 0)
        if (user_put(status_addr, exit_code))
            return _EFAULT;

    struct rusage_ rusage = task->group->rusage;
    // current->group is already locked
    rusage_add(&current->group->children_rusage, &rusage);
    if (rusage_addr != 0)
        if (user_put(rusage_addr, rusage))
            return _EFAULT;

    unlock(&task->group->lock);
    cond_destroy(&task->group->child_exit);
    free(task->group);
    task_destroy(task);
    return 1;
}

#define WNOHANG_ 1

dword_t sys_wait4(dword_t id, addr_t status_addr, dword_t options, addr_t rusage_addr) {
    STRACE("wait(%d, 0x%x, 0x%x, 0x%x)", id, status_addr, options, rusage_addr);
    lock(&pids_lock);
    int err;

retry:
    if (id == (dword_t) -1) {
        // look for a zombie child
        err = _ECHILD;
        if (list_empty(&current->children))
            goto error;
        struct task *task;
        list_for_each_entry(&current->children, task, siblings) {
            if (!task_is_leader(task))
                continue;
            id = task->pid;
            if (reap_if_zombie(task, status_addr, rusage_addr))
                goto found_zombie;
        }
    } else {
        // check if this child is a zombie
        struct task *task = pid_get_task_zombie(id);
        err = _ECHILD;
        if (task == NULL || task->parent->group != current->group)
            goto error;
        task = task->group->leader;
        if (reap_if_zombie(task, status_addr, rusage_addr))
            goto found_zombie;
    }

    err = _ECHILD;
    if (options & WNOHANG_)
        goto error;

    // no matching zombie found, wait for one
    err = _EINTR;
    if (wait_for(&current->group->child_exit, &pids_lock, NULL))
        goto error;
    goto retry;

found_zombie:
    unlock(&pids_lock);
    return id;
error:
    unlock(&pids_lock);
    return err;
}

dword_t sys_waitpid(dword_t pid, addr_t status_addr, dword_t options) {
    return sys_wait4(pid, status_addr, options, 0);
}
