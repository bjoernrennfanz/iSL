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

#include "uint128.h"
#include <assert.h>

#if defined(_MSC_VER)
typedef struct int128_t {
    uint32_t part[4];
} int128_t;
#else
typedef __int128 int128_t;
#endif

#define UINT128_NULLPTR ((void*)0)
int128_t uint128_to_int128(uint128_t x);
uint128_t int128_to_uint128(int128_t x);

#if defined(_MSC_VER)
int128_t uint128_to_int128(uint128_t x)
{
}

uint128_t int128_to_uint128(int128_t x)
{
}

uint128_t uint128_from_uint64(uint64_t x)
{
}

uint64_t uint128_to_uint64(bool *overflow, uint128_t x)
{
}

int uint128_compare(uint128_t x, uint128_t y)
{
}

int uint128_count_leading_zeros(uint128_t x)
{
}

uint128_t uint128_ones_complement(uint128_t x)
{
}

uint128_t uint128_and(uint128_t x, uint128_t y)
{
}

uint128_t uint128_or(uint128_t x, uint128_t y)
{
}

uint128_t uint128_shift_left(uint128_t x, int count)
{
}

uint128_t uint128_shift_right(uint128_t x, int count)
{
}

uint128_t uint128_add(bool *overflow, uint128_t x, uint128_t y)
{
}

int128_t int128_signed_add(bool *overflow, int128_t x, int128_t y)
{
}

uint128_t uint128_sub(bool *overflow, uint128_t x, uint128_t y)
{
}

uint128_t uint128_mul(bool *overflow, uint128_t x, uint128_t y)
{
}

#else

int128_t uint128_to_int128(uint128_t x)
{
    return (int128_t) x;
}

uint128_t int128_to_uint128(int128_t x)
{
    return (uint128_t) x;
}

uint128_t uint128_from_uint64(uint64_t x)
{
    return x;
}

uint64_t uint128_to_uint64(bool *overflow, uint128_t x)
{
    uint64_t result = (uint64_t)x;
    if (overflow)
    {
        *overflow = (result != x);
    }

    return result;
}

int uint128_compare(uint128_t x, uint128_t y)
{
    if (x < y)
    {
        return -1;
    }

    return x > y;
}

int uint128_count_leading_zeros(uint128_t x)
{
    uint128_t v = x;
    uint128_t mask = 1;

    mask <<= 127;

    int j;
    for(j = 0; j < 128 && 0 == (mask & v); j++)
    {
        mask >>= 1;
    }

    return j;
}

uint128_t uint128_ones_complement(uint128_t x)
{
    return ~x;
}

uint128_t uint128_and(uint128_t x, uint128_t y)
{
    return x & y;
}

uint128_t uint128_or(uint128_t x, uint128_t y)
{
    return x | y;
}

uint128_t uint128_shift_left(uint128_t x, int count)
{
    assert(count >= 0);
    return x << count;
}

uint128_t uint128_shift_right(uint128_t x, int count)
{
    assert(count >= 0);
    return x >> count;
}

uint128_t uint128_add(bool *overflow, uint128_t x, uint128_t y)
{
    uint128_t result = x + y;
    if (overflow)
    {
        *overflow = ((result < x) || (result < y));
    }

    return result;
}

int128_t int128_signed_add(bool *overflow, int128_t x, int128_t y)
{
    bool xs = x < 0, ys = y < 0;
    int128_t result = x + y;
    if (overflow)
    {
        *overflow = ((xs == ys) && (xs != (result < 0)));
    }

    return result;
}

uint128_t uint128_sub(bool *overflow, uint128_t x, uint128_t y)
{
    int128_t tmp = -(int128_t)y;
    return (uint128_t) int128_signed_add(overflow, (int128_t)x, tmp);
}

uint128_t uint128_mul(bool *overflow, uint128_t x, uint128_t y)
{
    uint128_t xh = x >> 64, xl = (uint64_t) x;
    uint128_t yh = y >> 64, yl = (uint64_t) y;

    uint128_t result = x * y;
    if (overflow)
    {
        *overflow = ((((((xl * yl) >> 64) + (xh * yl) + (yh * xl)) >> 64) + xh * yh) != 0);
    }

    return result;
}

#endif

uint128_t uint128_twos_complement(bool *overflow, uint128_t x)
{
    bool hasOverflow = false;

    uint128_t result = uint128_ones_complement(x);
    result = int128_to_uint128(int128_signed_add(&hasOverflow, uint128_to_int128(result), uint128_to_int128(uint128_from_uint64(1))));

    if (overflow)
    {
        *overflow = hasOverflow;
    }

    return result;
}

int128_t uint128_signed_sub(bool *overflow, int128_t x, int128_t y)
{
    int128_t tmp, result;
    bool hasOverflow = false;

    tmp = uint128_to_int128(uint128_twos_complement(&hasOverflow, int128_to_uint128(y)));
    result = int128_signed_add(&hasOverflow, x, tmp);

    if (overflow)
    {
        *overflow = hasOverflow;
    }

    return result;
}

uint128_t uint128_div(bool *overflow, uint128_t dividend, uint128_t divisor)
{
    uint128_t top, quotient, remainder;

    int bits = uint128_count_leading_zeros(dividend);
    bool allzero = uint128_compare(divisor, uint128_from_uint64(0)) == 0;

    quotient = uint128_from_uint64(0);
    remainder = uint128_from_uint64(0);

    top = uint128_shift_left(dividend, bits);
    for (; bits < 128; ++bits)
    {
        uint128_t tmp = remainder;
        remainder = uint128_add(UINT128_NULLPTR, tmp, tmp);
        tmp = top;

        bool addOverflow = false;
        top = uint128_add(&addOverflow, tmp, tmp);
        if (addOverflow)
        {
            tmp = remainder;
            uint128_add(UINT128_NULLPTR, tmp, uint128_from_uint64(1));
        }

        tmp = quotient;
        quotient = uint128_add(UINT128_NULLPTR, tmp, tmp);
        if (uint128_compare(remainder, divisor) >= 0)
        {
            tmp = quotient;
            quotient = uint128_add(UINT128_NULLPTR, tmp, uint128_from_uint64(1));
            tmp = remainder;
            remainder = int128_to_uint128(uint128_signed_sub(UINT128_NULLPTR, uint128_to_int128(tmp), uint128_to_int128(divisor)));
        }
    }

    if (*overflow)
    {
        *overflow = allzero;
    }

    return quotient;
}
