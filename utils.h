#ifndef __UTILS_H__
#define __UTILS_H__

#include <io.h>

#define NOTHING ((void *) -1)

void halt(const char* message, ...);
void memset(void *ptr, unsigned char value, size_t num);

unsigned long min(unsigned long x, unsigned long y);
unsigned long max(unsigned long x, unsigned long y);

#define assert(cond) if (!(cond)) halt("Assertion failed (%s) at %s:%s:%d\n", #cond, \
                                        __func__, __FILE__, __LINE__)   

#endif /* UTILS_H */

