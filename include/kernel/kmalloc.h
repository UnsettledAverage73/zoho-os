#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    size_t alloc_count;
    size_t free_count;
    size_t live_allocations;
    size_t bytes_used;
    size_t peak_bytes_used;
    size_t bytes_free;
    size_t largest_free_block;
    size_t failed_allocations;
} kmalloc_stats_t;

void kmalloc_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void kmalloc_get_stats(kmalloc_stats_t* out_stats);

#endif
