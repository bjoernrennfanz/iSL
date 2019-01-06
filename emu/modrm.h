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

#ifndef MODRM_H
#define MODRM_H

#include "util/debug.h"
#include "util/misc.h"
#include "emu/cpu.h"
#include "emu/tlb.h"

#undef DEFAULT_CHANNEL
#define DEFAULT_CHANNEL instr

struct modrm {
    union {
        enum reg32 reg;
        int opcode;
    };
    enum {
        modrm_reg, modrm_mem, modrm_mem_si
    } type;
    union {
        enum reg32 base;
        int rm_opcode;
    };
    int32_t offset;
    enum reg32 index;
    enum {
        times_1 = 0,
        times_2 = 1,
        times_4 = 2,
    } shift;
};

enum {
    rm_sib = reg_esp,
    rm_none = reg_esp,
    rm_disp32 = reg_ebp,
};
#define MOD(byte) ((byte & 0b11000000) >> 6)
#define REG(byte) ((byte & 0b00111000) >> 3)
#define RM(byte)  ((byte & 0b00000111) >> 0)

// read modrm and maybe sib, output information into *modrm, return false for segfault
static inline bool modrm_decode32(addr_t *ip, struct tlb *tlb, struct modrm *modrm) {
#define READ(thing) \
    if (!tlb_read(tlb, *ip, &(thing), sizeof(thing))) \
        return false; \
    *ip += sizeof(thing);

    byte_t modrm_byte;
    READ(modrm_byte);

    enum {
        mode_disp0,
        mode_disp8,
        mode_disp32,
        mode_reg,
    } mode = MOD(modrm_byte);
    modrm->type = modrm_mem;
    modrm->reg = REG(modrm_byte);
    modrm->base = RM(modrm_byte);
    if (mode == mode_reg) {
        modrm->type = modrm_reg;
    } else if (modrm->base == rm_disp32 && mode == mode_disp0) {
        modrm->base = reg_none;
        mode = mode_disp32;
    } else if (modrm->base == rm_sib && mode != mode_reg) {
        byte_t sib_byte;
        READ(sib_byte);
        modrm->base = RM(sib_byte);
        // wtf intel
        if (modrm->base == rm_disp32) {
            if (mode == mode_disp0) {
                modrm->base = reg_none;
                mode = mode_disp32;
            } else {
                modrm->base = reg_ebp;
            }
        }
        modrm->index = REG(sib_byte);
        modrm->shift = MOD(sib_byte);
        if (modrm->index != rm_none)
            modrm->type = modrm_mem_si;
    }

    if (mode == mode_disp0) {
        modrm->offset = 0;
    } else if (mode == mode_disp8) {
        int8_t offset;
        READ(offset);
        modrm->offset = offset;
    } else if (mode == mode_disp32) {
        int32_t offset;
        READ(offset);
        modrm->offset = offset;
    }
#undef READ

    TRACE("reg=%s opcode=%d ", reg32_name(modrm->reg), modrm->opcode);
    TRACE("base=%s ", reg32_name(modrm->base));
    if (modrm->type != modrm_reg)
        TRACE("offset=%s0x%x ", modrm->offset < 0 ? "-" : "", modrm->offset);
    if (modrm->type == modrm_mem_si)
        TRACE("index=%s<<%d ", reg32_name(modrm->index), modrm->shift);

    return true;
}

#endif
