#ifndef __DESC_H__
#define __DESC_H__

#include <stdint.h>
#include <memory.h>
#include <ioport.h>

struct desc_table_ptr {
	uint16_t size;
	uint64_t addr;
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

struct desc_table_entry {
    uint32_t e0;
    uint32_t e4;
    uint32_t e8;
    uint32_t e12;
} __attribute__((packed));

void interrupt_handler(uint64_t interruption_code)
{
    serial_port_print("interrupt handler ");
    serial_port_number(interruption_code);
    serial_port_print_char('\n');

    if (interruption_code == PIT_IDT_ENTRY)
    {
        out8(master_command_port, 1 << 5);    
    }
}

#define DESC_SYSTEM_NUMBER 32
#define DESC_NUMBER 48

extern uint64_t handler_wrappers[];

static struct desc_table_entry table[DESC_NUMBER];

static void initialize_idt()
{
    const uint32_t bits_0_15 = (1 << 16) - 1;
    const uint32_t bits_16_31 = (((uint64_t)1) << 32) - 1 - bits_0_15;

    for (int i = 0; i < DESC_NUMBER; i++) 
    {
        uint64_t offset = handler_wrappers[i];
        uint32_t segment_selector = KERNEL_CS; 

        table[i].e0 = (offset & bits_0_15) + (segment_selector << 16);

        // P
        table[i].e4 = (1 << 15);
        // TYPE
        table[i].e4 += (14 << 8);
        table[i].e4 += (offset & bits_16_31);
                                       
        table[i].e8 = (offset >> 32);

        table[i].e12 = 0;
    }

    struct desc_table_ptr pointer = {sizeof(table) - 1, (uint64_t)&table};
    write_idtr(&pointer);
}

void initialize_pic()
{
    //initialization command
    uint8_t command = 1 + (1 << 4);
    out8(master_command_port, command);
    out8(slave_command_port, command);

    //first idt entry
    out8(master_data_port, 32);
    out8(slave_data_port, 40);

    //configuration data
    out8(master_data_port, 1 << 2);
    out8(slave_data_port, 2);

    //misc functions data
    out8(master_data_port, 1);
    out8(slave_data_port, 1);

    //hide interruptions for pics
    out8(master_data_port, (1 << 8) - 1);
    out8(slave_data_port, (1 << 8) - 1);
}

#endif /*__DESC_H__*/
