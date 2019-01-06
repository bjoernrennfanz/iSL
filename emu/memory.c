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

#ifdef _WIN32
#   include "util/win32-unistd.h"
#   include "util/win32-mman.h"
#else
#   include <unistd.h>
#   include <sys/mman.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define DEFAULT_CHANNEL memory
#include "util/debug.h"
#include "kernel/errno.h"
#include "emu/memory.h"

// increment the change count
static void mem_changed(struct mem *mem);

struct mem *mem_new() {
    struct mem *mem = malloc(sizeof(struct mem));
    if (mem == NULL)
        return NULL;

    mem->refcount = 1;
    mem->pgdir = calloc(MEM_PGDIR_SIZE, sizeof(struct pt_entry *));
    mem->pgdir_used = 0;
    mem->changes = 0;
    mem->start_brk = mem->brk = 0; // should get overwritten by exec

    wrlock_init(&mem->lock);
    return mem;
}

void mem_retain(struct mem *mem) {
    mem->refcount++;
}

void mem_release(struct mem *mem) {
    if (--mem->refcount == 0) {
        write_wrlock(&mem->lock);
        pt_unmap(mem, 0, MEM_PAGES, PT_FORCE);

        for (int i = 0; i < MEM_PGDIR_SIZE; i++) {
            if (mem->pgdir[i] != NULL)
                free(mem->pgdir[i]);
        }
        free(mem->pgdir);
        write_wrunlock(&mem->lock);
        wrlock_destroy(&mem->lock);
        free(mem);
    }
}

#define PGDIR_TOP(page) ((page) >> 10)
#define PGDIR_BOTTOM(page) ((page) & (MEM_PGDIR_SIZE - 1))

static struct pt_entry *mem_pt_new(struct mem *mem, page_t page) {
    struct pt_entry *pgdir = mem->pgdir[PGDIR_TOP(page)];
    if (pgdir == NULL) {
        pgdir = mem->pgdir[PGDIR_TOP(page)] = calloc(MEM_PGDIR_SIZE, sizeof(struct pt_entry));
        mem->pgdir_used++;
    }
    return &pgdir[PGDIR_BOTTOM(page)];
}

struct pt_entry *mem_pt(struct mem *mem, page_t page) {
    struct pt_entry *pgdir = mem->pgdir[PGDIR_TOP(page)];
    if (pgdir == NULL)
        return NULL;
    struct pt_entry *entry = &pgdir[PGDIR_BOTTOM(page)];
    if (entry->data == NULL)
        return NULL;
    return entry;
}

static void mem_pt_del(struct mem *mem, page_t page) {
    struct pt_entry *entry = mem_pt(mem, page);
    if (entry != NULL)
        entry->data = NULL;
}

page_t pt_find_hole(struct mem *mem, pages_t size) {
    page_t hole_end;
    bool in_hole = false;
    for (page_t page = 0xf7ffd; page > 0x40000; page--) {
        // I don't know how this works but it does
        if (!in_hole && mem_pt(mem, page) == NULL) {
            in_hole = true;
            hole_end = page + 1;
        }
        if (mem_pt(mem, page) != NULL)
            in_hole = false;
        else if (hole_end - page == size)
            return page;
    }
    return BAD_PAGE;
}

bool pt_is_hole(struct mem *mem, page_t start, pages_t pages) {
    for (page_t page = start; page < start + pages; page++) {
        if (mem_pt(mem, page) != NULL)
            return false;
    }
    return true;
}

int pt_map(struct mem *mem, page_t start, pages_t pages, void *memory, unsigned flags) {
    if (memory == MAP_FAILED)
        return errno_map();

    struct data *data = malloc(sizeof(struct data));
    if (data == NULL)
        return _ENOMEM;
    data->data = memory;
    data->size = pages * PAGE_SIZE;
    data->refcount = 0;

    for (page_t page = start; page < start + pages; page++) {
        if (mem_pt(mem, page) != NULL)
            pt_unmap(mem, page, 1, 0);
        data->refcount++;
        struct pt_entry *pt = mem_pt_new(mem, page);
        pt->data = data;
        pt->offset = (page - start) << PAGE_BITS;
        pt->flags = flags;
    }
    return 0;
}

int pt_unmap(struct mem *mem, page_t start, pages_t pages, int force) {
    if (!force)
        for (page_t page = start; page < start + pages; page++)
            if (mem_pt(mem, page) == NULL)
                return -1;

    for (page_t page = start; page < start + pages; page++) {
        struct pt_entry *pt = mem_pt(mem, page);
        if (pt != NULL) {
            struct data *data = pt->data;
            mem_pt_del(mem, page);
            if (--data->refcount == 0) {
                munmap(data->data, data->size);
                free(data);
            }
        }
    }
    mem_changed(mem);
    return 0;
}

int pt_map_nothing(struct mem *mem, page_t start, pages_t pages, unsigned flags) {
    if (pages == 0) return 0;
    void *memory = mmap(NULL, pages * PAGE_SIZE,
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    return pt_map(mem, start, pages, memory, flags | P_ANON);
}

int pt_set_flags(struct mem *mem, page_t start, pages_t pages, int flags) {
    for (page_t page = start; page < start + pages; page++)
        if (mem_pt(mem, page) == NULL)
            return _ENOMEM;
    for (page_t page = start; page < start + pages; page++) {
        struct pt_entry *entry = mem_pt(mem, page);
        int old_flags = entry->flags;
        entry->flags = flags;
        // check if protection is increasing
        if ((flags & ~old_flags) & (P_READ|P_WRITE)) {
            void *data = (char *) entry->data->data + entry->offset;
            // force to be page aligned
            data = (void *) ((uintptr_t) data & ~(real_page_size - 1));
            int prot = PROT_READ;
            if (flags & P_WRITE) prot |= PROT_WRITE;
            if (mprotect(data, real_page_size, prot) < 0)
                return errno_map();
        }
    }
    mem_changed(mem);
    return 0;
}

int pt_copy_on_write(struct mem *src, page_t src_start, struct mem *dst, page_t dst_start, page_t pages) {
    for (page_t src_page = src_start, dst_page = dst_start;
            src_page < src_start + pages;
            src_page++, dst_page++) {
        struct pt_entry *entry = mem_pt(src, src_page);
        if (entry != NULL) {
            if (pt_unmap(dst, dst_page, 1, PT_FORCE) < 0)
                return -1;
            // TODO skip shared mappings
            entry->flags |= P_COW;
            entry->flags &= ~P_COMPILED;
            entry->data->refcount++;
            struct pt_entry *dst_entry = mem_pt_new(dst, dst_page);
            dst_entry->data = entry->data;
            dst_entry->offset = entry->offset;
            dst_entry->flags = entry->flags;
        }
    }
    mem_changed(src);
    mem_changed(dst);
    return 0;
}

static void mem_changed(struct mem *mem) {
    mem->changes++;
}

void *mem_ptr(struct mem *mem, addr_t addr, int type) {
    page_t page = PAGE(addr);
    struct pt_entry *entry = mem_pt(mem, page);

    if (entry == NULL) {
        // page does not exist
        // look to see if the next VM region is willing to grow down
        page_t p = page + 1;
        while (p < MEM_PAGES && mem_pt(mem, p) == NULL)
            p++;
        if (p >= MEM_PAGES)
            return NULL;
        if (!(mem_pt(mem, p)->flags & P_GROWSDOWN))
            return NULL;
        pt_map_nothing(mem, page, 1, P_WRITE | P_GROWSDOWN);
        entry = mem_pt(mem, page);
    }

    if (entry != NULL && type == MEM_WRITE) {
        // if page is unwritable, well tough luck
        if (!(entry->flags & P_WRITE))
            return NULL;
        // if page is cow, ~~milk~~ copy it
        if (entry->flags & P_COW) {
            void *data = (char *) entry->data->data + entry->offset;
            void *copy = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
            memcpy(copy, data, PAGE_SIZE);
            pt_map(mem, page, 1, copy, entry->flags &~ P_COW);
        }
    }

    if (entry == NULL)
        return NULL;
    return entry->data->data + entry->offset + PGOFFSET(addr);
}

size_t real_page_size;
__attribute__((constructor)) static void get_real_page_size() {
#ifdef _WIN32
    real_page_size = (size_t) getpagesize();
#else
    real_page_size = sysconf(_SC_PAGESIZE);
#endif
}
