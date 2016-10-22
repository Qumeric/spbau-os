#ifndef __BUDDY_ALLOCATOR_H__
#define __BUDDY_ALLOCATOR_H__

#include <memmap.h>

void buddy_allocator_init(struct memory_chunk chunks[], int chunks_n);
unsigned long buddy_allocate(int level);
void buddy_release(unsigned long phys_addr);

#endif /* __BUDDY_ALLOCATOR_H__ */

