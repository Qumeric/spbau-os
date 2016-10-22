#ifndef __DESC_H__
#define __DESC_H__

#include <stdint.h>
#include <memory.h>
#include <ioport.h>

struct desc_table_ptr {
	uint16_t size;
	uint64_t addr;
} __attribute__((packed));

struct desc_table_entry {
    uint32_t e0;
    uint32_t e4;
    uint32_t e8;
    uint32_t e12;
} __attribute__((packed));

static inline void read_idtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sidt %0" : "=m"(*ptr));
}

static inline void write_idtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lidt %0" : : "m"(*ptr));
}

static inline void read_gdtr(struct desc_table_ptr *ptr)
{
	__asm__ ("sgdt %0" : "=m"(*ptr));
}

static inline void write_gdtr(const struct desc_table_ptr *ptr)
{
	__asm__ ("lgdt %0" : : "m"(*ptr));
}
    
void initialize_idt();

void initialize_pic();

#endif /*__DESC_H__*/
