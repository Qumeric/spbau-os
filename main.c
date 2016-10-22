#include <desc.h>
#include <memmap.h>
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
 
    initialize_memory();  
}

