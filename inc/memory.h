#ifndef __MEMORY_H__
#define __MEMORY_H__

#define VIRTUAL_BASE	0xffffffff80000000
#define PAGE_SIZE	0x1000
#define KERNEL_CS	0x08
#define KERNEL_DS	0x10

#define master_command_port 0x20
#define master_data_port 0x21

#define slave_command_port 0xA0
#define slave_data_port 0xA1

#define serial_port 0x3f8

#define pit_0_data_port 0x40
#define pit_command_port 0x43

#define PIT_IDT_ENTRY 32 

#endif /* MEMORY_H */
