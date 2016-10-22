#include <memmap.h>
#include <multiboot.h>
#include <stdint.h>
#include <io.h>
#include <ints.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

extern const uint32_t multiboot_info;
extern const char text_phys_begin[];
extern const char bss_phys_end[];

void initialize_memory() 
{
    multiboot_info_t *mbi = (multiboot_info_t *) (unsigned long) multiboot_info;

    if (!CHECK_FLAG(mbi->flags, 6)) 
    {
        printf("Could not get memmap. Program will be halted.\n");
        disable_ints();
        __asm__("hlt");
    }
    
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)
                                   (unsigned long) mbi->mmap_addr;
    while ((unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length)
    {
        unsigned long first_byte = mmap->addr;
        unsigned long until_byte = first_byte + mmap->len;
            printf ("memory-range: 0x%x%x...0x%x%x type = 0x%x\n",
                    first_byte >> 32,
                    first_byte & 0xffffffff,
                    until_byte >> 32,
                    until_byte & 0xffffffff,
                    (unsigned) mmap->type);
            mmap = (multiboot_memory_map_t *) 
                ((unsigned long) mmap + mmap->size + sizeof (mmap->size));
    }

    {
        unsigned long kernel_first_byte = (unsigned long) text_phys_begin;
        unsigned long kernel_until_byte = (unsigned long) bss_phys_end;
        printf("kernel-range: 0x%x%x...0x%x%x\n",
               kernel_first_byte >> 32,
               kernel_first_byte & 0xffffffff,
               kernel_until_byte >> 32,
               kernel_until_byte & 0xffffffff); 
    }
}

