#include <ioport.h>

void serial_port_char(char c)
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

void initialize_serial_port() 
{
    // divisor coef
    out8(serial_port + 3, 1 << 7);
    out8(serial_port + 0, 1);
    out8(serial_port + 1, 0);

    // disable interrupts 
    out8(serial_port + 3, 0);
    out8(serial_port + 1, 0);

    // frame format
    out8(serial_port + 3, 7);
}

