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

#include <errno.h>
#include <limits.h>
#include "kernel/task.h"
#include "kernel/user-errno.h"
#include "util/sync.h"
#include "debug.h"
#ifdef _WIN32
#   include <windows.h>
#endif

void cond_init(cond_t *cond) {
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
#if __linux__
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
#endif
    pthread_cond_init(&cond->cond, &attr);
}

void cond_destroy(cond_t *cond) {
    pthread_cond_destroy(&cond->cond);
}

int wait_for(cond_t *cond, lock_t *lock, struct timespec *timeout) {
    if (current && current->pending)
        return _EINTR;
    int err = wait_for_ignore_signals(cond, lock, timeout);
    if (err < 0)
        return _ETIMEDOUT;
    if (current && current->pending)
        return _EINTR;
    return 0;
}

int wait_for_ignore_signals(cond_t *cond, lock_t *lock, struct timespec *timeout) {
    if (current) {
        lock(&current->waiting_cond_lock);
        current->waiting_cond = cond;
        current->waiting_lock = lock;
        unlock(&current->waiting_cond_lock);
    }
    int rc = 0;
    if (!timeout) {
        pthread_cond_wait(&cond->cond, &lock->m);
    } else {
#if defined(__linux__) || defined(_WIN32)
        struct timespec abs_timeout;
#if defined (__linux__)
        clock_gettime(CLOCK_MONOTONIC, &abs_timeout);
#else
        int64_t wintime;
        GetSystemTimeAsFileTime((FILETIME*)&wintime);

        wintime -=116444736000000000; //1jan1601 to 1jan1970
        abs_timeout.tv_sec = wintime / 10000000; //seconds
        abs_timeout.tv_nsec = wintime % 10000000 * 100; //nano-seconds
#endif

        abs_timeout.tv_sec += timeout->tv_sec;
        abs_timeout.tv_nsec += timeout->tv_nsec;
        if (abs_timeout.tv_nsec > 1000000000) {
            abs_timeout.tv_sec++;
            abs_timeout.tv_nsec -= 1000000000;
        }
        rc = pthread_cond_timedwait(&cond->cond, &lock->m, &abs_timeout);
#elif __APPLE__
        rc = pthread_cond_timedwait_relative_np(&cond->cond, &lock->m, timeout);
#else
#error Unimplemented pthread_cond_wait relative timeout.
#endif
    }
    if (current) {
        lock(&current->waiting_cond_lock);
        current->waiting_cond = NULL;
        current->waiting_lock = NULL;
        unlock(&current->waiting_cond_lock);
    }
    if (rc == ETIMEDOUT)
        return _ETIMEDOUT;
    return 0;
}

void notify(cond_t *cond) {
    pthread_cond_broadcast(&cond->cond);
}

void notify_once(cond_t *cond) {
    pthread_cond_signal(&cond->cond);
}

thread_local sigjmp_buf unwind_buf;
thread_local bool should_unwind = false;

void sigusr1_handler() {
    if (should_unwind) {
        should_unwind = false;
        siglongjmp(unwind_buf, 1);
    }
}
