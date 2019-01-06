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

#ifndef FLOAT80_H
#define FLOAT80_H

#include <stdint.h>
#include <stdbool.h>
#include "util/misc.h"

typedef struct {
    uint64_t signif;
    union {
        uint16_t signExp;
        struct {
            unsigned exp:15;
            unsigned sign:1;
        };
    };
} float80;

float80 f80_from_int(int64_t i);
int64_t f80_to_int(float80 f);
float80 f80_from_double(double d);
double f80_to_double(float80 f);

bool f80_isnan(float80 f);
bool f80_isinf(float80 f);
bool f80_iszero(float80 f);
bool f80_isdenormal(float80 f);
bool f80_is_supported(float80 f);

float80 f80_add(float80 a, float80 b);
float80 f80_sub(float80 a, float80 b);
float80 f80_mul(float80 a, float80 b);
float80 f80_div(float80 a, float80 b);
float80 f80_mod(float80 a, float80 b);
float80 f80_rem(float80 a, float80 b);

bool f80_lt(float80 a, float80 b);
bool f80_eq(float80 a, float80 b);
bool f80_uncomparable(float80 a, float80 b);

float80 f80_neg(float80 f);
float80 f80_abs(float80 f);

float80 f80_log2(float80 x);
float80 f80_sqrt(float80 x);

float80 f80_scale(float80 x, int scale);

enum f80_rounding_mode {
    round_to_nearest = 0,
    round_down = 1,
    round_up = 2,
    round_chop = 3,
};

extern thread_local enum f80_rounding_mode f80_rounding_mode;

#define F80_NAN ((float80) {.signif = 0xc000000000000000, .exp = 0x7fff, .sign = 0})
#define F80_INF ((float80) {.signif = 0x8000000000000000, .exp = 0x7fff, .sign = 0})

#endif
