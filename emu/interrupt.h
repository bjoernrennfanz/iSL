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

// Intel standard interrupts
// Any interrupt not handled specially becomes a SIGSEGV
#define INT_NONE -1
#define INT_DIV 0
#define INT_DEBUG 1
#define INT_NMI 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUND 5
#define INT_UNDEFINED 6
#define INT_FPU 7 // do not try to use the fpu. instead, try to realize the truth: there is no fpu.
#define INT_DOUBLE 8 // interrupt during interrupt, i.e. interruptception
#define INT_GPF 13
#define INT_TIMER 32
#define INT_SYSCALL 0x80

extern void handle_interrupt(int interrupt);
