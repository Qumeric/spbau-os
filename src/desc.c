#include <desc.h>
#include <io.h>

#define DESC_SYSTEM_NUMBER 32
#define DESC_NUMBER 48

extern uint64_t handler_wrappers[];

static struct desc_table_entry table[DESC_NUMBER];

void interrupt_handler(uint64_t interrupt_vector, uint64_t error_code)
{
    printf("Interrupt handler: ");
    if ((10 <= interrupt_vector && interrupt_vector <= 14)
       || 8 == interrupt_vector || interrupt_vector == 17)
    {
        printf("interrupt vector %llu with error code %llu\n", 
                interrupt_vector, error_code);
    }
    else
    {
        printf("interrupt vector %llu\n", interrupt_vector);
    }

    if (interrupt_vector == PIT_IDT_ENTRY)
    {
        out8(master_command_port, 1 << 5);    
    }
}

void initialize_idt()
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
}

