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

#define PUSH_REGISTERS     \
    __asm__("pushq %rax"); \
    __asm__("pushq %rbx"); \
    __asm__("pushq %rcx"); \
    __asm__("pushq %rdx"); \
    __asm__("pushq %rsi"); \
    __asm__("pushq %rdi"); \
    __asm__("pushq %rbp"); \
    __asm__("pushq %rsp"); \
    __asm__("pushq %r8");  \
    __asm__("pushq %r9");  \
    __asm__("pushq %r10"); \
    __asm__("pushq %r11"); \
    __asm__("pushq %r12"); \
    __asm__("pushq %r13"); \
    __asm__("pushq %r14"); \
    __asm__("pushq %r15");

#define POP_REGISTERS     \
    __asm__("popq %r15"); \
    __asm__("popq %r14"); \
    __asm__("popq %r13"); \
    __asm__("popq %r12"); \
    __asm__("popq %r11"); \
    __asm__("popq %r10"); \
    __asm__("popq %r9");  \
    __asm__("popq %r8");  \
    __asm__("popq %rsp"); \
    __asm__("popq %rbp"); \
    __asm__("popq %rdi"); \
    __asm__("popq %rsi"); \
    __asm__("popq %rdx"); \
    __asm__("popq %rcx"); \
    __asm__("popq %rbx"); \
    __asm__("popq %rax"); 

void interrupt_handler()
{
    char* mes = "Hello from interrupt handler!\n\0";
    serial_port_print(mes);
}

void interrupt_handler_pit()
{
    char* mes = "PIT interrupt handler.\n\0";
    serial_port_print(mes);

    out8(master_command_port, 1 << 5);
}

void interrupt_wrapper_with_error()
{
    __asm__("add $8, %rsp");
    PUSH_REGISTERS
    __asm__("call interrupt_handler");
    POP_REGISTERS
    __asm__("iretq");
}

void interrupt_wrapper_pit()
{
    PUSH_REGISTERS
    __asm__("call interrupt_handler_pit");
    POP_REGISTERS
    __asm__("iretq");
}

void interrupt_wrapper_without_error()
{
    PUSH_REGISTERS
    __asm__("call interrupt_handler");
    POP_REGISTERS
    __asm__("iretq");
}

#define DESC_SYSTEM_NUMBER 32
#define DESC_NUMBER 48

static struct desc_table_entry table[DESC_NUMBER];

static void initialize_idt()
{
    const uint32_t bits_0_15 = (1 << 16) - 1;
    const uint32_t bits_16_31 = (((uint64_t)1) << 32) - 1 - bits_0_15;

    for (int i = 0; i < DESC_NUMBER; i++) 
    {
        uint64_t offset;
        if (i == 8 || (10 <= i && i <= 14) || i == 17)
        {
            offset = (uint64_t)&interrupt_wrapper_with_error;
        } 
        else if (i == 32)
        {
            offset = (uint64_t)&interrupt_wrapper_pit;
        }
        else 
        {
            offset = (uint64_t)&interrupt_wrapper_without_error;
        }
        uint32_t segment_selector = KERNEL_CS; 

        table[i].e0 = (offset & bits_0_15) + (segment_selector << 16);

        // P
        table[i].e4 = (1 << 15);
        // DPL
        if (i < DESC_SYSTEM_NUMBER)
        {
            table[i].e4 += (3 << 13);
        }                                                                               // TYPE
        table[i].e4 += (14 << 8);
                                                                                        table[i].e4 += (offset & bits_16_31);
                                       
        table[i].e8 = (offset >> 32);

        table[i].e12 = 0;
    }

    struct desc_table_ptr pointer;
    pointer.size = DESC_NUMBER * 16 - 1;
    pointer.addr = (uint64_t)&table;
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
