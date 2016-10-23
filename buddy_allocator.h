#ifndef __BUDDY_ALLOCATOR_H__
#define __BUDDY_ALLOCATOR_H__

#include <memmap.h>
#include <stdint.h>

struct buddy_descriptor 
{
    uint8_t level;
    uint8_t state;
    int32_t prev;
    int32_t next;
};

void buddy_allocator_init(struct memory_chunk chunks[], int chunks_n);
unsigned long buddy_allocate(int level);
void buddy_release(unsigned long phys_addr);

#endif /* __BUDDY_ALLOCATOR_H__ */

