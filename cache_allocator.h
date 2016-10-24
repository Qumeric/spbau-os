#ifndef __CACHE_ALLOCATOR_H__
#define __CACHE_ALLOCATOR_H__

#include <stdint.h>

struct cache_node_info
{
    struct cache_node_info *next;
};

struct cache_allocator
{
    uint16_t size;
    struct cache_node_info *head;
};

struct cache_allocator *cache_allocator_init(uint16_t size);
void cache_allocator_deinit(struct cache_allocator *allocator);

void *cache_allocator_alloc(struct cache_allocator *allocator);
void cache_allocator_free(struct cache_allocator *allocator, void *ptr);

#endif /* __CACHE_ALLOCATOR_H__ */

