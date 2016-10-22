#include <memmap.h>
#include <multiboot.h>
#include <stdint.h>
#include <io.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const uint32_t multiboot_info;

void add_to_memmap_kernel() 
{
    multiboot_info_t *mbi = (multiboot_info_t *) multiboot_info;

    if (CHECK_FLAG(mbi->flags, 6)) 
    {
        multiboot_memory_map_t *mmap = mbi->mmap_addr;
        while ((unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length)
        {
            unsigned long first_byte = mmap->addr;
            unsigned long last_byte = first_byte + mmap->len - 1;
            printf ("memory-range: 0x%x%x-0x%x%x type = 0x%x\n",               
                    first_byte >> 32,                                                               first_byte & 0xffffffff,
                    last_byte >> 32,
                    last_byte & 0xffffffff,
                    (unsigned) mmap->type);
            mmap = (multiboot_memory_map_t *) 
                ((unsigned long) mmap + mmap->size + sizeof (mmap->size));
        } 
    }
}

