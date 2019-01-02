#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include "emu/cpu.h"
#include "kernel/fs.h"
#include "kernel/signal.h"
#include "kernel/resource.h"
#include "util/list.h"
#include "util/timer.h"
#include "util/sync.h"

// everything here is private to the thread executing this task and needs no
// locking, unless otherwise specified
struct task {
    struct cpu_state cpu;
    struct mem *mem;
    pthread_t thread;
    uint64_t threadid;

    struct tgroup *group; // immutable
    struct list group_links;
    pid_t_ pid, tgid; // immutable
    uid_t_ uid, gid;
    uid_t_ euid, egid;
    uid_t_ suid, sgid;
#define MAX_GROUPS 32
    int ngroups;
    uid_t_ groups[MAX_GROUPS];
    char comm[16];
    bool did_exec; // for that one annoying setsid edge case

    struct fdtable *files;
    struct fs_info *fs;

    struct sighand *sighand;
    sigset_t_ blocked;
    sigset_t_ queued; // where blocked signals go when they're sent
    sigset_t_ pending;

    // locked by pids_lock
    struct task *parent;
    struct list children;
    struct list siblings;

    addr_t clear_tid;

    // locked by parent's thread group
    dword_t exit_code;
    bool zombie;

    // I wish conditions variables were as reliable as wait queues. alas, they are not
    bool vfork_done;
    cond_t vfork_cond;
    lock_t vfork_lock;
    int exit_signal;

    // lock for anything that needs locking but is not covered by some other lock
    // right now, just comm
    lock_t general_lock;

    // current condition/lock, so it can be notified in case of a signal
    cond_t *waiting_cond;
    lock_t *waiting_lock;
    lock_t waiting_cond_lock;
};

// current will always give the process that is currently executing
// if I have to stop using __thread, current will become a macro
extern __thread struct task *current;

// Creates a new process, initializes most fields from the parent. Specify
// parent as NULL to create the init process. Returns NULL if out of memory.
// Ends with an underscore because there's a mach function by the same name
struct task *task_create_(struct task *parent);
// Removes the process from the process table and frees it. Must be called with pids_lock.
void task_destroy(struct task *task);

void vfork_notify(struct task *task);

// struct thread_group is way too long to type comfortably
struct tgroup {
    struct list threads;
    struct task *leader; // immutable
    struct rusage_ rusage;

    // locked by pids_lock
    pid_t_ sid, pgid;
    struct list session;
    struct list pgroup;

    struct tty *tty;

    bool has_timer;
    struct timer *timer;

    struct rlimit_ limits[RLIMIT_NLIMITS_];

    // https://twitter.com/tblodt/status/957706819236904960
    // TODO locking
    bool doing_group_exit;
    dword_t group_exit_code;

    struct rusage_ children_rusage;
    cond_t child_exit;

    lock_t lock;
};

static inline bool task_is_leader(struct task *task) {
    return task->group->leader == task;
}

struct pid {
    dword_t id;
    struct task *task;
    struct list session;
    struct list pgroup;
};

// synchronizes obtaining a pointer to a task and freeing that task
extern lock_t pids_lock;
// these functions must be called with pids_lock
struct pid *pid_get(dword_t pid);
struct task *pid_get_task(dword_t pid);
struct task *pid_get_task_zombie(dword_t id); // don't return null if the task exists as a zombie

#define MAX_PID (1 << 10) // oughta be enough

// When a thread is created to run a new process, this function is used.
extern void (*task_run_hook)(void);
// TODO document
void task_start(struct task *task);

extern void (*exit_hook)(int code);

#define superuser() (current->euid == 0)

#endif
