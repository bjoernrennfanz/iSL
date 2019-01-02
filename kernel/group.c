#include "util/list.h"
#include "kernel/calls.h"
#include "kernel/task.h"

dword_t sys_setpgid(dword_t id, dword_t pgid) {
    STRACE("setpgid(%d, %d)", id, pgid);
    int err;
    if (id == 0)
        id = current->pid;
    if (pgid == 0)
        pgid = id;
    lock(&pids_lock);
    struct pid *pid = pid_get(id);
    struct task *task = pid->task;
    err = _ESRCH;
    if (task == NULL)
        goto out;
    struct tgroup *tgroup = task->group;

    // you can only join a process group in the same session
    if (id != pgid) {
        // there has to be a process in pgrp that's in the same session as id
        err = _EPERM;
        struct pid *group_pid = pid_get(pgid);
        if (group_pid == NULL || list_empty(&group_pid->pgroup))
            goto out;
        struct tgroup *group_first_tgroup = list_first_entry(&group_pid->pgroup, struct tgroup, pgroup);
        if (tgroup->sid != group_first_tgroup->sid)
            goto out;
    }

    // you can only change the process group of yourself or a child
    err = _ESRCH;
    if (task != current && task->parent != current)
        goto out;
    // a session leader cannot create a process group
    err = _EPERM;
    if (tgroup->sid == tgroup->leader->pid)
        goto out;

    // TODO cannot set process group of a child that has done exec

    if (tgroup->pgid != pgid) {
        list_remove(&tgroup->pgroup);
        tgroup->pgid = pgid;
        list_add(&pid->pgroup, &tgroup->pgroup);
    }

    err = 0;
out:
    unlock(&pids_lock);
    return err;
}

dword_t sys_setpgrp() {
    return sys_setpgid(0, 0);
}

dword_t sys_getpgid(dword_t pid) {
    STRACE("getpgid(%d)", pid);
    if (pid != 0)
        return _EPERM;
    lock(&pids_lock);
    pid_t_ pgid = current->group->pgid;
    unlock(&pids_lock);
    return pgid;
}
dword_t sys_getpgrp() {
    return sys_getpgid(0);
}

dword_t sys_setsid() {
    lock(&pids_lock);
    struct tgroup *group = current->group;
    pid_t_ new_sid = group->leader->pid;
    if (group->pgid == new_sid || group->sid == new_sid) {
        unlock(&pids_lock);
        return _EPERM;
    }

    struct pid *pid = pid_get(current->pid);
    list_remove_safe(&group->session);
    list_add(&pid->session, &group->session);
    group->sid = new_sid;
    list_remove_safe(&group->pgroup);
    list_add(&pid->pgroup, &group->pgroup);
    group->pgid = new_sid;

    lock(&group->lock);
    group->tty = NULL;
    unlock(&group->lock);

    unlock(&pids_lock);
    return new_sid;
}

dword_t sys_getsid() {
    lock(&pids_lock);
    pid_t_ sid = current->group->sid;
    unlock(&pids_lock);
    return sid;
}

