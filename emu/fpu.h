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

#ifndef EMU_FPU_H
#define EMU_FPU_H

#include <math.h>
#include "emu/float80.h"

struct cpu_state;

typedef float float32;
typedef double float64;

enum fpu_const {
    fconst_one = 0,
    fconst_log2t = 1,
    fconst_log2e = 2,
    fconst_pi = 3,
    fconst_log2 = 4,
    fconst_ln2 = 5,
    fconst_zero = 6,
};

static float80 fpu_consts[] = {
    [fconst_one]   = (float80) {.signif = 0x8000000000000000, .signExp = 0x3fff},
    [fconst_log2t] = (float80) {.signif = 0xd49a784bcd1b8afe, .signExp = 0x4000},
    [fconst_log2e] = (float80) {.signif = 0xb8aa3b295c17f0bc, .signExp = 0x3fff},
    [fconst_pi]    = (float80) {.signif = 0xc90fdaa22168c235, .signExp = 0x4000},
    [fconst_log2]  = (float80) {.signif = 0x9a209a84fbcff799, .signExp = 0x3ffd},
    [fconst_ln2]   = (float80) {.signif = 0xb17217f7d1cf79ac, .signExp = 0x3ffe},
    [fconst_zero]  = (float80) {.signif = 0x0000000000000000, .signExp = 0x0000},
};

void fpu_pop(struct cpu_state *cpu);
void fpu_xch(struct cpu_state *cpu, int i);

void fpu_st(struct cpu_state *cpu, int i);
void fpu_ist16(struct cpu_state *cpu, int16_t *i);
void fpu_ist32(struct cpu_state *cpu, int32_t *i);
void fpu_ist64(struct cpu_state *cpu, int64_t *i);
void fpu_stm32(struct cpu_state *cpu, float *f);
void fpu_stm64(struct cpu_state *cpu, double *f);
void fpu_stm80(struct cpu_state *cpu, float80 *f);

void fpu_ld(struct cpu_state *cpu, int i);
void fpu_ldc(struct cpu_state *cpu, enum fpu_const c);
void fpu_ild16(struct cpu_state *cpu, int16_t *i);
void fpu_ild32(struct cpu_state *cpu, int32_t *i);
void fpu_ild64(struct cpu_state *cpu, int64_t *i);
void fpu_ldm32(struct cpu_state *cpu, float *f);
void fpu_ldm64(struct cpu_state *cpu, double *f);
void fpu_ldm80(struct cpu_state *cpu, float80 *f);

void fpu_prem(struct cpu_state *cpu);
void fpu_rndint(struct cpu_state *cpu);
void fpu_scale(struct cpu_state *cpu);
void fpu_abs(struct cpu_state *cpu);
void fpu_chs(struct cpu_state *cpu);
void fpu_sqrt(struct cpu_state *cpu);
void fpu_yl2x(struct cpu_state *cpu);
void fpu_2xm1(struct cpu_state *cpu);

void fpu_com(struct cpu_state *cpu, int i);
void fpu_comm32(struct cpu_state *cpu, float *f);
void fpu_comm64(struct cpu_state *cpu, double *f);
void fpu_comi(struct cpu_state *cpu, int i);
void fpu_tst(struct cpu_state *cpu);
#define fpu_ucom fpu_com
#define fpu_ucomi fpu_comi

void fpu_add(struct cpu_state *cpu, int srci, int dsti);
void fpu_sub(struct cpu_state *cpu, int srci, int dsti);
void fpu_subr(struct cpu_state *cpu, int srci, int dsti);
void fpu_mul(struct cpu_state *cpu, int srci, int dsti);
void fpu_div(struct cpu_state *cpu, int srci, int dsti);
void fpu_divr(struct cpu_state *cpu, int srci, int dsti);
void fpu_iadd16(struct cpu_state *cpu, int16_t *i);
void fpu_isub16(struct cpu_state *cpu, int16_t *i);
void fpu_isubr16(struct cpu_state *cpu, int16_t *i);
void fpu_imul16(struct cpu_state *cpu, int16_t *i);
void fpu_idiv16(struct cpu_state *cpu, int16_t *i);
void fpu_idivr16(struct cpu_state *cpu, int16_t *i);
void fpu_iadd32(struct cpu_state *cpu, int32_t *i);
void fpu_isub32(struct cpu_state *cpu, int32_t *i);
void fpu_isubr32(struct cpu_state *cpu, int32_t *i);
void fpu_imul32(struct cpu_state *cpu, int32_t *i);
void fpu_idiv32(struct cpu_state *cpu, int32_t *i);
void fpu_idivr32(struct cpu_state *cpu, int32_t *i);
void fpu_addm32(struct cpu_state *cpu, float *f);
void fpu_subm32(struct cpu_state *cpu, float *f);
void fpu_subrm32(struct cpu_state *cpu, float *f);
void fpu_mulm32(struct cpu_state *cpu, float *f);
void fpu_divm32(struct cpu_state *cpu, float *f);
void fpu_divrm32(struct cpu_state *cpu, float *f);
void fpu_addm64(struct cpu_state *cpu, double *f);
void fpu_subm64(struct cpu_state *cpu, double *f);
void fpu_subrm64(struct cpu_state *cpu, double *f);
void fpu_mulm64(struct cpu_state *cpu, double *f);
void fpu_divm64(struct cpu_state *cpu, double *f);
void fpu_divrm64(struct cpu_state *cpu, double *f);

void fpu_stcw16(struct cpu_state *cpu, uint16_t *i);
void fpu_ldcw16(struct cpu_state *cpu, uint16_t *i);
void fpu_patan(struct cpu_state *cpu);
void fpu_xam(struct cpu_state *cpu);

#endif
