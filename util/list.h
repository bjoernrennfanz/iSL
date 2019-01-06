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

#ifndef LIST_H
#define LIST_H

#if defined(_MSC_VER)
#   include <io.h>
#else
#   include <unistd.h>
#endif
#include <stdbool.h>
#include <stddef.h>

struct list {
    struct list *next, *prev;
};

static inline void list_init(struct list *list) {
    list->next = list;
    list->prev = list;
}

static inline bool list_null(struct list *list) {
    return list->next == NULL && list->prev == NULL;
}

static inline bool list_empty(struct list *list) {
    return list->next == list || list_null(list);
}

static inline void _list_add_between(struct list *prev, struct list *next, struct list *item) {
    prev->next = item;
    item->prev = prev;
    item->next = next;
    next->prev = item;
}

static inline void list_add_before(struct list *list, struct list *item) {
    _list_add_between(list->prev, list, item);
}

static inline void list_add(struct list *list, struct list *item) {
    _list_add_between(list, list->next, item);
}

static inline void list_init_add(struct list *list, struct list *item) {
    if (list_null(list))
        list_init(list);
    list_add(list, item);
}

static inline void list_remove(struct list *item) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = item->prev = NULL;
}

static inline void list_remove_safe(struct list *item) {
    if (!list_null(item))
        list_remove(item);
}

#define list_entry(item, type, member) \
    ((type *) ((char *) (item) - offsetof(type, member)))
#define list_first_entry(list, type, member) \
    list_entry((list)->next, type, member)
#define list_next_entry(item, member) \
    list_entry((item)->member.next, typeof(*(item)), member)

#define list_for_each(list, item) \
    for (item = (list)->next; item != (list); item = item->next)
#define list_for_each_safe(list, item, tmp) \
    for (item = (list)->next, tmp = item->next; item != (list); \
            item = tmp, tmp = item->next)

#define list_for_each_entry(list, item, member) \
    for (item = list_entry((list)->next, typeof(*item), member); \
            &item->member != (list); \
            item = list_entry(item->member.next, typeof(*item), member))
#define list_for_each_entry_safe(list, item, tmp, member) \
    for (item = list_first_entry(list, typeof(*(item)), member), \
            tmp = list_next_entry(item, member); \
            &item->member != (list); \
            item = tmp, tmp = list_next_entry(item, member))

static inline unsigned long list_size(struct list *list) {
    unsigned long count = 0;
    struct list *item;
    list_for_each(list, item) {
        count++;
    }
    return count;
}

#endif
