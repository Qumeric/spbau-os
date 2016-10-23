#ifndef __PAGING_H__
#define __PAGING_H__

#include <memmap.h>

void pagging_create_mapping(struct memory_chunk *chunks, int chunks_n);

#endif /* __PAGING_H__ */

