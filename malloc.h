#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdint.h>

void init_malloc_mechanisms();
void *malloc(unsigned long size);
void free(void *ptr);

#endif /* __MALLOC_H__ */

