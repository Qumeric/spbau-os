#include <buddy_allocator.h>
#include <stdint.h>
#include <memory.h>
#include <utils.h>

#define BUDDY_FREE 0
#define BUDDY_USED 1

#define LEVELS 30
#define VOID -1

struct buddy_list
{
    int32_t head;
};

static struct buddy_list lists[LEVELS];

static struct buddy_descriptor *descs;
int number_of_descriptors;

static void list_add(struct buddy_list *list, int32_t num)
{
    assert(0 <= num && num < number_of_descriptors);
    if (list->head != VOID)
    {
        descs[list->head].prev = num;
    }
    descs[num].prev = VOID;
    descs[num].next = list->head;
    list->head = num;
}

static void list_remove(int32_t num)
{
    assert(0 <= num && num < number_of_descriptors);
    int level = descs[num].level;
    assert(0 <= level && level < LEVELS);
    if (descs[num].prev != VOID)
    {
        descs[descs[num].prev].next = descs[num].next; 
    }
    if (descs[num].next != VOID)
    {
        descs[descs[num].next].prev = descs[num].prev;
    }
    if (lists[level].head == num)
    {
        lists[level].head = descs[num].next;
    }
}

void buddy_allocator_init(struct memory_chunk chunks[], int chunks_n)
{
    printf("Initializing buddy allocator...\n");
    if (chunks_n == 0)
    {
        halt("There is no memory for buddy allocator's usage.");
    }

    number_of_descriptors = 0; 
    for (int i = 0; i < chunks_n; i++)
    {
        number_of_descriptors = max(number_of_descriptors, chunks[i].until / PAGE_SIZE);
    }
    unsigned long bytes_needed = 
        ((unsigned long) sizeof(struct buddy_descriptor)) * number_of_descriptors;
    printf("Buddy allocator needs %d descriptors or %lu bytes\n",
            number_of_descriptors, bytes_needed);

    // finding memory for descriptors in the first 4GB
    int chunk_to_use = VOID; 
    for (int i = 0; i < chunks_n; i++)
    {
        if (chunks[i].first + bytes_needed <= chunks[i].until &&
            chunks[i].first + bytes_needed <= (((unsigned long) 1) << 32))
        {
            chunk_to_use = i;
            break;
        }    
    }
    if (chunk_to_use == VOID)
    {
        halt("Not enough memory for descriptors.");
    }
    printf("Memory for descriptors is found at 0x%lx...0x%lx\n", 
            chunks[chunk_to_use].first,
            chunks[chunk_to_use].first + bytes_needed);
    descs = (struct buddy_descriptor *) chunks[chunk_to_use].first;
    chunks[chunk_to_use].first += bytes_needed;

    for (int i = 0; i < number_of_descriptors; i++)
    {
        descs[i].state = BUDDY_USED;
    }
    for (int i = 0; i < chunks_n; i++)
    {
        unsigned long pointer = chunks[i].first + PAGE_SIZE - 1;
        pointer -= pointer % PAGE_SIZE;
        while (pointer + PAGE_SIZE <= chunks[i].until)
        {
            int desc_number = pointer / PAGE_SIZE;
            descs[desc_number].level = 0; 
            descs[desc_number].state = BUDDY_FREE;
            pointer += PAGE_SIZE;
        } 
    }
   
    //merging
    for (int level = 1; level < LEVELS; level++)
    {
        for (int i = 0; i < number_of_descriptors; i += (1 << level))
        {
            int bud = i + (1 << (level - 1));
            if (bud < number_of_descriptors &&
                descs[i].level == level - 1  && descs[bud].level == level - 1 &&
                descs[i].state == BUDDY_FREE && descs[bud].state == BUDDY_FREE)
            {
                descs[i].level = level;
                descs[bud].state = BUDDY_USED;               
            }
        }
    }
  
    for (int i = 0; i < LEVELS; i++)
    {
        lists[i].head = VOID;
    }
    for (int i = 0; i < number_of_descriptors; i++)
    {
        if (descs[i].state == BUDDY_FREE)
        {
            list_add(lists + descs[i].level, i);           
        }
    }
    printf("Buddy allocator's initialization is finished.\n\n");
}

static void buddy_make_non_empty(int level)
{
    if (lists[level].head != VOID)
    {
        return;
    }
    // make non empty last level -- impossible 
    if (level + 1 == LEVELS)
    {
        return;
    }
    
    buddy_make_non_empty(level + 1);
    // could not make non empty next level
    if (lists[level + 1].head == VOID)
    {
        return;
    }

    int desc_number = lists[level + 1].head;
    list_remove(desc_number);
    descs[desc_number].level = level;
    descs[desc_number + (1 << level)].state = BUDDY_FREE;
    list_add(lists + level, desc_number);
    list_add(lists + level, desc_number + (1 << level)); 
}

unsigned long buddy_allocate(int level)
{
    buddy_make_non_empty(level);
    if (lists[level].head == VOID)
    {
        return -1;
    }
    int32_t result = lists[level].head;
    list_remove(result);
    descs[result].state = BUDDY_USED;
    return ((unsigned long) PAGE_SIZE) * result;
}

static void buddy_add_and_merge(int32_t desc_num)
{
    int level = descs[desc_num].level;
    // it is possible to merge
    if (level + 1 != LEVELS)
    {
        int32_t bud_desc_num = desc_num ^ (1 << level);
        if (bud_desc_num < number_of_descriptors &&
            descs[bud_desc_num].state == BUDDY_FREE && descs[bud_desc_num].level == level)
        {
            list_remove(bud_desc_num);
            int32_t smaller = min(desc_num, bud_desc_num);
            int32_t larger = max(desc_num, bud_desc_num);
            descs[smaller].level = level + 1;
            descs[larger].state = BUDDY_USED;
            buddy_add_and_merge(smaller);
            return;
        }
    }
    // have not merged it yet
    list_add(lists + level, desc_num);
}

void buddy_release(unsigned long phys_addr)
{
    assert(phys_addr % PAGE_SIZE == 0);
    int32_t desc_number = phys_addr / PAGE_SIZE;
    assert(desc_number < number_of_descriptors);
    assert(descs[desc_number].state == BUDDY_USED);
    descs[desc_number].state = BUDDY_FREE;
    buddy_add_and_merge(desc_number);
}

