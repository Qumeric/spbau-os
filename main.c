#include <desc.h>
#include <memmap.h>
#include <ioport.h>
#include <pit.h>
#include <ints.h>
#include <buddy_allocator.h>
#include <cache_allocator.h>
#include <utils.h>

void cache_test(uint16_t size)
{
    struct cache_allocator *allocator = cache_allocator_init(size);
    int cnt = 0;
    for (int i = 1; i <= 100; i++)
    {
        void *memory = cache_allocator_alloc(allocator);
        if (memory == (void *) -1)
        {
            break;
        }
        printf("ALLOC:0x%lx ", memory);
        cache_allocator_free(allocator, memory);
        printf("FREED ");
        memory = cache_allocator_alloc(allocator);
        if (memory == (void *) -1)
        {
            assert(0);
        }
        printf("ALLOC:0x%lx\n", memory);
        cnt++;
    }
    printf("CACHE ALLOCATOR CREATED %d pointers of %d bytes\n", cnt, size);
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

    //testing cache_allocator
    cache_test(10);
    cache_test(1000);
   
    while (1);
}

