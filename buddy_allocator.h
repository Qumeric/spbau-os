#ifndef __BUDDY_ALLOCATOR_H__
#define __BUDDY_ALLOCATOR_H__

#include <memmap.h>
#include <stdint.h>

#define NOT_FOR_CACHE 0

struct buddy_descriptor 
{
    uint8_t level;
    uint8_t state;
    uint16_t cache_size;
    int32_t prev;
    int32_t next;
};

void buddy_allocator_init(struct memory_chunk chunks[], int chunks_n);
void *buddy_allocate(int level, uint16_t cache_size);
void buddy_release(void *logical);
struct buddy_descriptor *buddy_allocator_get_descriptor(void *logical);

#endif /* __BUDDY_ALLOCATOR_H__ */

