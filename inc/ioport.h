#ifndef __IOPORT_H__
#define __IOPORT_H__

#include <stdint.h>
#include <bit.h>
#include <memory.h>

static inline void out8(unsigned short port, uint8_t data)
{ __asm__ volatile("outb %0, %1" : : "a"(data), "d"(port)); }

static inline uint8_t in8(unsigned short port)
{
	uint8_t value;

	__asm__ volatile("inb %1, %0" : "=a"(value) : "d"(port));
	return value;
}

static inline void out16(unsigned short port, uint16_t data)
{ __asm__ volatile("outw %0, %1" : : "a"(data), "d"(port)); }

static inline uint16_t in16(unsigned short port)
{
	uint16_t value;

	__asm__ volatile("inw %1, %0" : "=a"(value) : "d"(port));
	return value;
}

static inline void out32(unsigned short port, uint32_t data)
{ __asm__ volatile("outl %0, %1" : : "a"(data), "d"(port)); }

static inline uint32_t in32(unsigned short port)
{
	uint32_t value;

	__asm__ volatile("inl %1, %0" : "=a"(value) : "d"(port));
	return value;
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
}

#endif /* __IOPORT_H__ */
