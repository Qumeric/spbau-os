#include <desc.h>
#include <memmap.h>
#include <ioport.h>
#include <pit.h>
#include <ints.h>
#include <buddy_allocator.h>
#include <utils.h>

void main(void)
{
    disable_ints();    

    initialize_serial_port();
    initialize_idt();
    initialize_pic();
    // initialize_pit();

    enable_ints();
 
    initialize_memory();  

    //testing buddy allocator
    unsigned long addr2 = buddy_allocate(2);
    printf("ALLOCATED: 0x%lx...0x%lx\n", addr2, addr2 + (1 << 2) * PAGE_SIZE);
    unsigned long addr3 = buddy_allocate(3);
    printf("ALLOCATED: 0x%lx...0x%lx\n", addr3, addr3 + (1 << 3) * PAGE_SIZE);
    buddy_release(addr2);
    printf("RELEASED: 0x%lx...0x%lx\n", addr2, addr2 + (1 << 2) * PAGE_SIZE);
    buddy_release(addr3);
    printf("RELEASED: 0x%lx...0x%lx\n", addr3, addr3 + (1 << 3) * PAGE_SIZE);
    addr2 = buddy_allocate(2);
    printf("ALLOCATED: 0x%lx...0x%lx\n", addr2, addr2 + (1 << 2) * PAGE_SIZE);
    addr3 = buddy_allocate(3);
    printf("ALLOCATED: 0x%lx...0x%lx\n", addr3, addr3 + (1 << 3) * PAGE_SIZE);
    buddy_release(addr2);
    printf("RELEASED: 0x%lx...0x%lx\n", addr2, addr2 + (1 << 2) * PAGE_SIZE);
    buddy_release(addr3);
    printf("RELEASED: 0x%lx...0x%lx\n", addr3, addr3 + (1 << 3) * PAGE_SIZE);
}

