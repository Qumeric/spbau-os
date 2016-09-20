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

void main(void)
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

    char *mes = "Hello, world!\n\0";
    serial_port_print(mes);  
}
