#ifndef __HEAP_H__
#define __HEAP_H__

#include <common.h>

typedef u8 hb_table_entry;

struct heap_table
{
    hb_table_entry *entries;
    szt total;
};

struct heap
{
    struct heap_table *table;
    void *start_addr;
};

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table);
void* heap_malloc(struct heap *heap, szt size);
void heap_free(struct heap *heap, void *ptr);

#endif