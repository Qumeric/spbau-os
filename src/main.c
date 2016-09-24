#include <desc.h>
#include <memory.h>
#include <ioport.h>
#include <pit.h>
#include <ints.h>

void main(void)
{
    disable_ints();    

    initialize_serial_port();

    initialize_idt();

    initialize_pic();

    initialize_pit();

    enable_ints();

    __asm__("int $3");

    int wait = 1;
    while (wait);
}
