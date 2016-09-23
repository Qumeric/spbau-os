#include <desc.h>
#include <memory.h>
#include <ioport.h>
#include <pit.h>

void main(void)
{
    __asm__("cli");

    initialize_serial_port();

    initialize_idt();

    initialize_pic();

    initialize_pit();

    __asm__("sti");

    __asm__("int $3");

    int wait = 1;
    while (wait);
}
