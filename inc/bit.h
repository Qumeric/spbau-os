#ifndef __BIT_H__
#define __BIT_H__

#include <stdint.h>

static void setbit(uint8_t *data, int bit) 
{
    *data |= (1 << bit);
}

static void clearbit(uint8_t *data, int bit) 
{
    *data &= ~(1 << bit);
}

#endif /* BIT_H */
