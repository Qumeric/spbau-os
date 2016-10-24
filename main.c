#include <desc.h>
#include <memmap.h>
#include <ioport.h>
#include <pit.h>
#include <ints.h>
#include <buddy_allocator.h>
#include <cache_allocator.h>
#include <malloc.h>
#include <utils.h>

void test_malloc(int size)
{
    printf("TEST MALLOC FOR int[%d]\n", size);
    int *numbers = (int *) malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
    {
        numbers[i] = i;
    }
    int sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += numbers[i];
    }
    free(numbers);
    printf("FINISHED SUCCESSFULLY\n");
}

void main(void)
{
    disable_ints();    

    initialize_serial_port();
    initialize_idt();
    initialize_pic();
    // initialize_pit();

    enable_ints();
 
    initialize_memory();  

    // testing malloc
    test_malloc(100);
    test_malloc(10000);
 
    while (1);
}

