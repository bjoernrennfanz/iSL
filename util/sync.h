/*
 * iSL (Subsystem for Linux) for iOS & Android
 * Based on iSH (https://ish.app)
 *
 * Copyright (C) 2018 - 2019 Björn Rennfanz (bjoern@fam-rennfanz.de)
 * Copyright (C) 2017 - 2019 Theodore Dubois (tblodt@icloud.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTIL_SYNC_H
#define UTIL_SYNC_H

#if defined(_MSC_VER)
#   include "util/msvc-stdatomic.h"
#   include "util/msvc-pthread.h"
#else
#   include <stdatomic.h>
#   include <pthread.h>
#endif
#include "util/misc.h"

#include <stdbool.h>
#include <setjmp.h>

// locks, implemented using pthread

typedef struct {
    pthread_mutex_t m;
    pthread_t owner;
} lock_t;
#define lock_init(lock) pthread_mutex_init(&(lock)->m, NULL)
#define LOCK_INITIALIZER {PTHREAD_MUTEX_INITIALIZER, 0}
#define lock(lock) do { pthread_mutex_lock(&(lock)->m); (lock)->owner = pthread_self(); } while (0)
#define unlock(lock) pthread_mutex_unlock(&(lock)->m)

// conditions, implemented using pthread conditions but hacked so you can also
// be woken by a signal

typedef struct {
    pthread_cond_t cond;
} cond_t;

// Must call before using the condition
void cond_init(cond_t *cond);
// Must call when finished with the condition (currently doesn't do much but might do something important eventually I guess)
void cond_destroy(cond_t *cond);
// Releases the lock, waits for the condition, and reacquires the lock.
// Returns _EINTR if waiting stopped because the thread received a signal,
// _ETIMEDOUT if waiting stopped because the timout expired, 0 otherwise.
int wait_for(cond_t *cond, lock_t *lock, struct timespec *timeout);
// Same as wait_for, except it will never return _EINTR
int wait_for_ignore_signals(cond_t *cond, lock_t *lock, struct timespec *timeout);
// Wake up all waiters.
void notify(cond_t *cond);
// Wake up one waiter.
void notify_once(cond_t *cond);

// this is a read-write lock that prefers writers, i.e. if there are any
// writers waiting a read lock will block.
// on darwin pthread_rwlock_t is already like this, on linux you can configure
// it to prefer writers. not worrying about anything else right now.
typedef pthread_rwlock_t wrlock_t;
static inline void wrlock_init(wrlock_t *lock) {
    pthread_rwlockattr_t *pattr = NULL;
#if defined(__GLIBC__)
    pthread_rwlockattr_t attr;
    pattr = &attr;
    pthread_rwlockattr_init(pattr);
    pthread_rwlockattr_setkind_np(pattr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
#endif
    pthread_rwlock_init(lock, pattr);
}
#define wrlock_destroy(lock) pthread_rwlock_destroy(lock)
#define read_wrlock(lock) pthread_rwlock_rdlock(lock)
#define read_wrunlock(lock) pthread_rwlock_unlock(lock)
#define write_wrlock(lock) pthread_rwlock_wrlock(lock)
#define write_wrunlock(lock) pthread_rwlock_unlock(lock)

extern thread_local sigjmp_buf unwind_buf;
extern thread_local bool should_unwind;
static inline int sigunwind_start() {
    if (sigsetjmp(unwind_buf, 1)) {
        should_unwind = true;
        return 1;
    } else {
        return 0;
    }
}
static inline void sigunwind_end() {
    should_unwind = false;
}

#endif
