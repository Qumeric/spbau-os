#include <desc.h>
#include <ioport.h>

static const unsigned short serial_port = 0x3f8;

static void setbit(uint8_t *data, int bit) 
{
    *data |= (1 << bit);
}

static void clearbit(uint8_t *data, int bit) 
{
    *data &= ~(1 << bit);
}

static void serial_port_print(char *mes) 
{
    int i = 0;
    while (mes[i] != '\0')
    {
        uint8_t p5 = in8(serial_port + 5);
        if (p5 & (1 << 5)) 
        {
            out8(serial_port + 0, mes[i++]);
        }
    }
}

static void initialize_serial_port() 
{
    // div
    uint8_t p3 = in8(serial_port + 3);
    setbit(&p3, 7); 
    out8(serial_port + 3, p3);
    out8(serial_port + 0, 1);
    out8(serial_port + 1, 0);

    // disable interrupts 
    p3 = in8(serial_port + 3);
    clearbit(&p3, 7);
    out8(serial_port + 3, p3);
    out8(serial_port + 1, 0);

    // frame format
    p3 = in8(serial_port + 3);
    setbit(&p3, 0);
    setbit(&p3, 1);
    clearbit(&p3, 2);
    clearbit(&p3, 3);
    clearbit(&p3, 7);
    out8(serial_port + 3, p3);
}

struct desc_table_entry {
    uint32_t e0;
    uint32_t e4;
    uint32_t e8;
    uint32_t e12;
} __attribute__((packed));

#define DESC_SYSTEM_NUMBER 32
#define DESC_NUMBER 48

static const uint32_t bits_0_15 = (uint64_t(1) >> 16) - 1;
static const uint32_t bits_16_31 = (uint64_t(1) >> 32) - 1 - bits_0_15;

struct desc_table_entry table[DESC_NUMBER];

static void interrupt_handler() 
{
    char *mes = "Hello, IDT\n\0";
    serial_port_print(mes);   
}

static void initialize_idt()
{
    for (int i = 0; i < DESC_NUMBER; i++) 
    {
        uint64_t offset = (uint64_t)&interrupt_handler;
        uint16_t segment_selector = KERNEL_CS; 
        
        table[i].e0 = segment_selector + ((offset & bits_0_15) >> 16);

        table[i].e4 = ((offset & bits_16_31) << 16);
        // P
        table[i].e4 += (1 << 16);
        // DPL
        if (i < DESC_SYSTEM_NUMBER)
        {
            table[i].e4 += (uint32_t(3) << 17);
        }
        // TYPE
        table[i].e4 += (uint32_t(14) << 20);

        table[i].e8 = (offset << 32);

        table[i].e12 = 0;
    }     

    struct desc_table_ptr pointer;
    pointer.size = DESC_NUMBER * 16 - 1;
    pointer.addr = (uint64_t)&table;
    write_idtr(&pointer);
}

void main(void)
{
    initialize_serial_port();

    int wait = 1;
    while (wait);
    
    initialize_idt();
}
