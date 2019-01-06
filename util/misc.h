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

#ifndef MISC_H
#define MISC_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#   define noreturn __declspec(noreturn)
#else
#   include <stdnoreturn.h>
#endif
#include <sys/types.h>

// utility macros
#define glue(a, b) _glue(a, b)
#define _glue(a, b) a##b
#define glue3(a,b,c) glue(a, glue(b, c))
#define glue4(a,b,c,d) glue(a, glue3(b, c, d))

#define str(x) _str(x)
#define _str(x) #x

// keywords
#define bits unsigned int

#if defined(_MSC_VER)
#   define forceinline __forceinline
#else
#   define forceinline inline __attribute__((always_inline))
#endif

#if defined(_MSC_VER)
#   define flatten
#else
#   define flatten __attribute__((flatten))
#endif

#ifdef NDEBUG
#   if defined(_MSC_VER)
#       define postulate __assume
#   else
#       define postulate __builtin_assume
#   endif
#else
#   define postulate assert
#endif

#define typecheck(type, x) ({type _x = x; x;})

#if defined(_MSC_VER)
#   define must_check _Check_return_
#else
#   define must_check __attribute__((warn_unused_result))
#endif

#if defined(_MSC_VER)
#   define __no_instrument
#else
#   if defined(__has_attribute) && __has_attribute(no_sanitize)
#       define __no_instrument __attribute__((no_sanitize("address", "thread", "undefined", "leak", "memory")))
#   else
#       define __no_instrument
#   endif
#endif

#if defined(_MSC_VER)
#   define thread_local __declspec(thread)
#else
#   define thread_local __thread
#endif

#if defined(__x86_64__)
#define rdtsc() ({ \
        uint32_t low, high; \
        __asm__ volatile("rdtsc" : "=a" (high), "=d" (low)); \
        ((uint64_t) high) << 32 | low; \
    })
#elif defined(__arm64__) || defined(__aarch64__)
#define rdtsc() ({ \
        uint64_t tsc; \
        __asm__ volatile("mrs %0, PMCCNTR_EL0" : "=r" (tsc)); \
        tsc; \
    })
#endif

// types
typedef int64_t sqword_t;
typedef uint64_t qword_t;
typedef uint32_t dword_t;
typedef int32_t sdword_t;
typedef uint16_t word_t;
typedef uint8_t byte_t;

typedef dword_t addr_t;
typedef dword_t uint_t;
typedef sdword_t int_t;

typedef sdword_t pid_t_;
typedef dword_t uid_t_;
typedef word_t mode_t_;
typedef sqword_t off_t_;

#define uint(size) glue3(uint,size,_t)
#define sint(size) glue3(int,size,_t)

#define ERR_PTR(err) (void *) (intptr_t) (err)
#define PTR_ERR(ptr) (intptr_t) (ptr)
#define IS_ERR(ptr) ((uintptr_t) (ptr) > (uintptr_t) -0xfff)

#ifdef _WIN32
#define sigjmp_buf jmp_buf
#define sigsetjmp(env, savemask) setjmp(env)
#define siglongjmp(env, val) longjmp(env, val)
#define sigemptyset(x) (void)0
#endif

#endif
