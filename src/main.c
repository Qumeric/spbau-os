#include <desc.h>
#include <memory.h>
#include <ioport.h>

static const unsigned short serial_port = 0x3f8;

static const unsigned short master_command_port = 0x20;
static const unsigned short master_data_port = 0x21;

static const unsigned short slave_command_port = 0xA0;
static const unsigned short slave_data_port = 0xA1;

static void setbit(uint8_t *data, int bit) 
{
    *data |= (1 << bit);
}

static void clearbit(uint8_t *data, int bit) 
{
    *data &= ~(1 << bit);
}

static void serial_port_print_char(char c)
{
    while (1) 
    {
        uint8_t p5 = in8(serial_port + 5);
        if (p5 & (1 << 5)) 
        {
            out8(serial_port + 0, c);
            break;
        }
    }
}

static void serial_port_print(char *mes) 
{
    for (int i = 0; mes[i] != 0; i++) 
    {
        serial_port_print_char(mes[i]);
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

static struct desc_table_entry table[DESC_NUMBER];

void interrupt_handler() 
{
    char* mes = "Hello from interrupt handler!\n\0";
    serial_port_print(mes);
}

static void pnumber(uint64_t x) 
{
    int a[70];
    int cnt = 0;
    while (x > 0) 
    {
        a[cnt++] = x % 10;
        x /= 10;
    }
    while (cnt > 0) 
    {
        serial_port_print_char('0' + a[--cnt]);
    }
    serial_port_print_char('\n');
}

static void pbit(uint64_t x)
{
    int a[64];
    for (int i = 0; i < 64; i++)
    {
        a[i] = x % 2;
        x /= 2;
    }
    for (int i = 63; i >= 0; i--) 
    { 
        serial_port_print_char('0' + a[i]);
        if (i % 16 == 0) 
        {
            serial_port_print_char(' ');
        }
    }
    serial_port_print_char('\n');
}

void interrupt_wrapper()
{
    __asm__("pushq %rax");
    __asm__("pushq %rbx");
    __asm__("pushq %rcx");
    __asm__("pushq %rdx");
    __asm__("pushq %rsi");
    __asm__("pushq %rdi");
    __asm__("pushq %rbp");
    __asm__("pushq %rsp");
    __asm__("pushq %r8");
    __asm__("pushq %r9");
    __asm__("pushq %r10");
    __asm__("pushq %r11");
    __asm__("pushq %r12");
    __asm__("pushq %r13");
    __asm__("pushq %r14");
    __asm__("pushq %r15");

    __asm__("call interrupt_handler");
    
    __asm__("popq %r15");
    __asm__("popq %r14");
    __asm__("popq %r13"); 
    __asm__("popq %r12"); 
    __asm__("popq %r11"); 
    __asm__("popq %r10"); 
    __asm__("popq %r9"); 
    __asm__("popq %r8"); 
    __asm__("popq %rsp"); 
    __asm__("popq %rbp"); 
    __asm__("popq %rdi"); 
    __asm__("popq %rsi"); 
    __asm__("popq %rdx"); 
    __asm__("popq %rcx"); 
    __asm__("popq %rbx"); 
    __asm__("popq %rax");   

    __asm__("iretq");
}

static void initialize_idt()
{
    const uint32_t bits_0_15 = (1 << 16) - 1;
    const uint32_t bits_16_31 = (((uint64_t)1) << 32) - 1 - bits_0_15;

    for (int i = 0; i < DESC_NUMBER; i++) 
    {
        uint64_t offset = (uint64_t)&interrupt_wrapper;
        uint32_t segment_selector = KERNEL_CS; 

        table[i].e0 = (offset & bits_0_15) + (segment_selector << 16);

        // P
        table[i].e4 = (1 << 15);
        // DPL
        if (i < DESC_SYSTEM_NUMBER)
        {
            table[i].e4 += (3 << 13);
        }
        // TYPE
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

static void initialize_pic()
{
    //initialization command
    uint8_t command = 1 + (1 << 4);
    out8(master_command_port, command);
    out8(slave_command_port, command);

    //first idt entry
    out8(master_data_port, 32);
    out8(slave_data_port, 40);

    //configuaration data
    out8(master_data_port, 1 << 2);
    out8(slave_data_port, 2);

    //misc functions data
    out8(master_data_port, 1);
    out8(slave_data_port, 1); 

    //hide interruptions for pics
    out8(master_data_port, (1 << 8) - 1);
    out8(slave_data_port, (1 << 8) - 1);
}

void main(void)
{
    __asm__("cli");

    initialize_serial_port();

    initialize_idt();

    initialize_pic();

    __asm__("sti");

    __asm__("int $0");
}
