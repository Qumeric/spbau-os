#include <malloc.h>
#include <memory.h>
#include <buddy_allocator.h>
#include <cache_allocator.h>
#include <utils.h>

#define CACHE_CNT 12

static struct cache_allocator *small_allocators[CACHE_CNT];

void init_malloc_mechanisms()
{
    printf("Starting malloc mechanisms...\n");
    for (int i = 0; i < CACHE_CNT; i++)
    {
        small_allocators[i] = cache_allocator_init(1 << i);
        if (small_allocators[i] == NOTHING)
        {
            halt("Not enough memory for initializing malloc mechanisms");
        }
    }
    printf("Finished with malloc initialization.\n\n");
}

static unsigned char get_not_less_power(unsigned long size)
{
    unsigned char i = 0;
    while ((1ull << i) < size)
    {
        i++;
    }
    return i;
}

void *malloc(unsigned long size)
{
    unsigned char power = get_not_less_power(size);
    if (power < CACHE_CNT)
    {
        return cache_allocator_alloc(small_allocators[power]);
    }
    else
    {
        return buddy_allocate(power - CACHE_CNT, NOT_FOR_CACHE);
    } 
}

void free(void *ptr)
{
    unsigned long ptrl = (unsigned long) ptr;
    void *aligned = (void *) (ptrl - ptrl % PAGE_SIZE);
    struct buddy_descriptor *desc = buddy_allocator_get_descriptor(aligned);

    if (desc->cache_size == NOT_FOR_CACHE)
    {
        buddy_release(ptr);
    }
    else
    {
        unsigned char power = get_not_less_power(desc->cache_size);
        assert(power < CACHE_CNT); 
        cache_allocator_free(small_allocators[power], ptr);
    }
}

