/*
 * iSL (Subsystem for Linux) for iOS & Android
 *
 * Copyright (C) 2018 Björn Rennfanz (bjoern@fam-rennfanz.de)
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

#ifndef UINT128_H
#define UINT128_H

#include <stdbool.h>
#include <stdint.h>

#if defined(_MSC_VER)
typedef struct uint128_t {
  uint32_t part[4];
} uint128_t;
#else
typedef unsigned __int128 uint128_t;
#endif

uint128_t uint128_from_uint64(uint64_t x);
uint64_t uint128_to_uint64(bool *overflow, uint128_t x);

/* Returns the usual (-1, 0, 1) for (LT, EQ, GT) */
int uint128_compare(uint128_t x, uint128_t y);

int uint128_count_leading_zeros(uint128_t x);

uint128_t uint128_ones_complement(uint128_t x);
uint128_t uint128_twos_complement(bool *overflow, uint128_t x);

uint128_t uint128_and(uint128_t x, uint128_t y);
uint128_t uint128_or(uint128_t x, uint128_t y);
uint128_t uint128_shift_left(uint128_t x, int count);
uint128_t uint128_shift_right(uint128_t x, int count);

uint128_t uint128_add(bool *overflow, uint128_t x, uint128_t y);
uint128_t uint128_sub(bool *overflow, uint128_t x, uint128_t y);
uint128_t uint128_mul(bool *overflow, uint128_t x, uint128_t y);
uint128_t uint128_div(bool *overflow, uint128_t x, uint128_t y);

#endif // UINT128_H
