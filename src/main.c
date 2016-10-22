#include <desc.h>
#include <memory.h>
#include <ioport.h>
#include <pit.h>
#include <ints.h>
#include <memmap.h>

void main(void)
{
    disable_ints();    

    initialize_serial_port();
    initialize_idt();
    initialize_pic();
    initialize_pit();

    enable_ints();
 
    add_to_memmap_kernel();  
}
