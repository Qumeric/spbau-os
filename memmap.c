#include <memmap.h>
#include <memory.h>
#include <multiboot.h>
#include <stdint.h>
#include <io.h>
#include <utils.h>
#include <buddy_allocator.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const uint32_t multiboot_info;
extern const char text_phys_begin[];
extern const char bss_phys_end[];

#define MAX_CHUNKS 32

void add_chunk(unsigned long first, unsigned long until, 
               struct memory_chunk chunks[], int *chunks_free)
{
    chunks[*chunks_free].first = first;
    chunks[*chunks_free].until = until;
    (*chunks_free)++;
}

void initialize_memory() 
{
    printf("Getting memory map from multiboot...\n");
    multiboot_info_t *mbi = (multiboot_info_t *) (unsigned long) multiboot_info;

    if (!CHECK_FLAG(mbi->flags, 6)) 
    {
        halt("Could not get memmap.\n");
    }

    unsigned long kernel_first_byte = (unsigned long) text_phys_begin;
    unsigned long kernel_until_byte = (unsigned long) bss_phys_end;
    printf("kernel-range: 0x%lx...0x%lx\n", 
           kernel_first_byte, 
           kernel_until_byte);

    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)
                                   (unsigned long) mbi->mmap_addr;    

    struct memory_chunk chunks[MAX_CHUNKS];
    int chunks_free = 0;

    while ((unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length)
    {
        unsigned long first_byte = mmap->addr;
        unsigned long until_byte = first_byte + mmap->len;
            printf ("memory-range: 0x%lx...0x%lx type = 0x%x\n",
                    first_byte,
                    until_byte,
                    mmap->type);
        if (mmap->type == 1) 
        {
            unsigned long first_intersect = max(first_byte, kernel_first_byte);
            unsigned long until_intersect = min(until_byte, kernel_until_byte);
            
            // have an intersection
            if (first_intersect < until_intersect)
            {
                // have something before the intersection
                if (first_byte < first_intersect)
                { 
                    add_chunk(first_byte, first_intersect, 
                              chunks, &chunks_free); 
                }
            
                // ... after ...
                if (until_intersect < until_byte)
                { 
                    add_chunk(until_intersect, until_byte, 
                              chunks, &chunks_free); 
                }
            }
            else 
            {
                add_chunk(first_byte, until_byte, chunks, &chunks_free);
            }
        }
        
        mmap = (multiboot_memory_map_t *) 
               ((unsigned long) mmap + mmap->size + sizeof (mmap->size));
    }
    printf("Free chunks are:\n");
    for (int i = 0; i < chunks_free; i++) 
    {
        printf("0x%lx...0x%lx\n", chunks[i].first, chunks[i].until);
    }
    printf("\n");

    buddy_allocator_init(chunks, chunks_free);   
}

