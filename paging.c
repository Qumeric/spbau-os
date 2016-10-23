#include <paging.h>
#include <utils.h>
#include <buddy_allocator.h>
#include <memory.h>

#define TABLE_ENTRIES 512
#define TABLE_SIZE 4096

unsigned long *create_table()
{
    void *addr = buddy_allocate(0);    
    memset(addr, 0, TABLE_SIZE);
    assert(((unsigned long) addr) % PAGE_SIZE == 0);
    return (unsigned long *) addr;
}

#define PML4E_SIZE  (1ul << 39)
#define PDPTE_SIZE  (1ul << 30)
#define PDTE_SIZE   (1ul << 21)
#define PDE_SIZE    (1ul << 12)

#define P_FLAG  (1 << 0)
#define RW_FLAG (1 << 1)
#define PS_FLAG (1 << 7)

static unsigned long get_pml4(unsigned long logical)
{
    return (logical >> 39) & 0x1ff;
}

static unsigned long get_pdpt(unsigned long logical)
{
    return (logical >> 30) & 0x1ff;
}

static unsigned long get_pdt(unsigned long logical)
{
    return (logical >> 21) & 0x1ff;
}

static unsigned long get_pd(unsigned long logical)
{
    return (logical >> 12) & 0x1ff;
}

// assumes logical adress is in the from -- 111...111000...000
static void map(unsigned long *table, unsigned long entry_size, 
                unsigned long logical, unsigned long physical, unsigned long size)
{
    printf("table=0x%lx, entry_size=0x%lx, logical=0x%lx, physical=0x%lx, size=0x%lx\n",
            table, entry_size, logical, physical, size);
    assert(size != 0);
    assert(entry_size != 0);
    assert(size % PAGE_SIZE == 0);

    while (size >= entry_size)
    {
        if (entry_size == PDPTE_SIZE)
        {
            unsigned long *new_table = create_table(); 
            table[get_pdpt(logical)] = 
                P_FLAG | RW_FLAG  | (((unsigned long) new_table) - SHIFTED_BASE);
            for (int i = 0; i < TABLE_ENTRIES; i++)
            {
                new_table[i] = P_FLAG | RW_FLAG | PS_FLAG | (physical + i * PDTE_SIZE);
            }
        }
        else if (entry_size == PDTE_SIZE)
        {
            table[get_pdt(logical)] = P_FLAG | RW_FLAG | PS_FLAG | physical;
        }
        else if (entry_size == PDE_SIZE) 
        {
            table[get_pd(logical)] = P_FLAG | RW_FLAG | physical;
        }
        else
        {
            assert(0);
        }
 
        size -= entry_size;   
        logical += entry_size;
        physical += entry_size;
    } 

    if (size == 0)
    {
        return;
    }

    unsigned long *new_table = create_table();
    unsigned long i;
    if (entry_size == PML4E_SIZE)
    {
        i = get_pml4(logical);
    }
    else if (entry_size == PDPTE_SIZE)
    {
        i = get_pdpt(logical);
    }
    else if (entry_size == PDTE_SIZE)
    {
        i = get_pdt(logical);
    }
    else
    {
        assert(0);
    }
    table[i] = P_FLAG | RW_FLAG | (((unsigned long) new_table) - SHIFTED_BASE);
    map(new_table, entry_size / TABLE_ENTRIES, logical, physical, size);
}


void pagging_create_mapping(unsigned long memory_available)
{
    printf("Starting mapping logical addresses to physical...\n"); 
    unsigned long *pml4 = create_table();

    map(pml4, PML4E_SIZE, SHIFTED_BASE, 0, memory_available);
    map(pml4, PML4E_SIZE, VIRTUAL_BASE, 0, 1ul << 31);

    unsigned long addr_to_write = ((unsigned long) pml4) - SHIFTED_BASE;
    __asm__ volatile ("movq %0, %%cr3" : : "a"(addr_to_write) : "memory");
    printf("Finished mapping.\n\n");
}

