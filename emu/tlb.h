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

#ifndef TLB_H
#define TLB_H

#include <string.h>
#include "emu/memory.h"
#include "util/debug.h"

struct tlb_entry {
    page_t page;
    page_t page_if_writable;
    uintptr_t data_minus_addr;
};
#define TLB_BITS 10
#define TLB_SIZE (1 << TLB_BITS)
struct tlb {
    struct mem *mem;
    page_t dirty_page;
    struct tlb_entry entries[TLB_SIZE];
};

#define TLB_INDEX(addr) (((addr >> PAGE_BITS) & (TLB_SIZE - 1)) ^ (addr >> (PAGE_BITS + TLB_BITS)))
#define TLB_PAGE(addr) (addr & 0xfffff000)
#define TLB_PAGE_EMPTY 1
void tlb_init(struct tlb *tlb, struct mem *mem);
void tlb_free(struct tlb *tlb);
void tlb_flush(struct tlb *tlb);
void *tlb_handle_miss(struct tlb *tlb, addr_t addr, int type);

forceinline __no_instrument void *__tlb_read_ptr(struct tlb *tlb, addr_t addr) {
    struct tlb_entry entry = tlb->entries[TLB_INDEX(addr)];
    if (entry.page == TLB_PAGE(addr)) {
        void *address = (void *) (entry.data_minus_addr + addr);
        postulate(address != NULL);
        return address;
    }
    return tlb_handle_miss(tlb, addr, MEM_READ);
}
bool __tlb_read_cross_page(struct tlb *tlb, addr_t addr, char *out, unsigned size);
forceinline __no_instrument bool tlb_read(struct tlb *tlb, addr_t addr, void *out, unsigned size) {
    if (PGOFFSET(addr) > PAGE_SIZE - size)
        return __tlb_read_cross_page(tlb, addr, out, size);
    void *ptr = __tlb_read_ptr(tlb, addr);
    if (ptr == NULL)
        return false;
    memcpy(out, ptr, size);
    return true;
}

forceinline __no_instrument void *__tlb_write_ptr(struct tlb *tlb, addr_t addr) {
    struct tlb_entry entry = tlb->entries[TLB_INDEX(addr)];
    if (entry.page_if_writable == TLB_PAGE(addr)) {
        tlb->dirty_page = TLB_PAGE(addr);
        void *address = (void *) (entry.data_minus_addr + addr);
        postulate(address != NULL);
        return address;
    }
    return tlb_handle_miss(tlb, addr, MEM_WRITE);
}
bool __tlb_write_cross_page(struct tlb *tlb, addr_t addr, const char *value, unsigned size);
forceinline __no_instrument bool tlb_write(struct tlb *tlb, addr_t addr, const void *value, unsigned size) {
    if (PGOFFSET(addr) > PAGE_SIZE - size)
        return __tlb_write_cross_page(tlb, addr, value, size);
    void *ptr = __tlb_write_ptr(tlb, addr);
    if (ptr == NULL)
        return false;
    memcpy(ptr, value, size);
    return true;
}

#endif
