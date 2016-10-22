#ifndef __MEMMAP_H__
#define __MEMMAP_H__

typedef unsigned long memory_chunk_addr;

struct memory_chunk {
    memory_chunk_addr first;
    memory_chunk_addr until;
};

void initialize_memory();

#endif /* MEMMAP_H */

