#include <buddy_allocator.h>
#include <stdint.h>
#include <memory.h>
#include <utils.h>

#define BUDDY_FREE 0
#define BUDDY_USED 1

struct buddy_list
{
    int32_t head;
};


#define LEVELS 20
#define NUMBER_OF_DESCRIPTORS (1 << LEVELS)

#define VOID -1

static struct buddy_descriptor descs[NUMBER_OF_DESCRIPTORS];
static struct buddy_list lists[LEVELS];

static void list_add(struct buddy_list *list, int32_t num)
{
    assert((0 <= num) && (num < NUMBER_OF_DESCRIPTORS));
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
    assert((0 <= num) && (num < NUMBER_OF_DESCRIPTORS));
    int level = descs[num].level;
    assert((0 <= level) && (level < LEVELS));
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
    for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i++)
    {
        descs[i].state = BUDDY_USED;
    }

    int pages_created = 0;
    for (int i = 0; i < chunks_n; i++)
    {
        unsigned long pointer = chunks[i].first + PAGE_SIZE - 1;
        pointer -= pointer % PAGE_SIZE;
        while (pointer + PAGE_SIZE <= chunks[i].until)
        {
            int desc_number = pointer / PAGE_SIZE;
            if (desc_number >= NUMBER_OF_DESCRIPTORS)
            {
                halt_program("Can not handle so much memory.");
            }
            descs[desc_number].level = 0; 
            descs[desc_number].state = BUDDY_FREE;
            pages_created++;
            pointer += PAGE_SIZE;
        } 
    }
    printf("%d pages of size %d are successfully created\n", pages_created, PAGE_SIZE);
    
    //merging
    for (int level = 1; level < LEVELS; level++)
    {
        for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i += (1 << level))
        {
            int bud = i + (1 << (level - 1));
            if (descs[i].level == level - 1  && descs[bud].level == level - 1 &&
                descs[i].state == BUDDY_FREE && descs[bud].state == BUDDY_FREE)
            {
                descs[i].level = level;
                descs[bud].state = BUDDY_USED;               
            }
        }
    }
   
    // cnt array is just for verifying correctness of merging  
    int cnt[LEVELS];
    for (int i = 0; i < LEVELS; i++)
    {
        cnt[i] = 0;
    }
    for (int i = 0; i < NUMBER_OF_DESCRIPTORS; i++)
    {
        if (descs[i].state == BUDDY_FREE)
        {
            int level = descs[i].level;
            cnt[level]++;
            list_add(lists + level, i);           
        }
    }
    int sum = 0;
    for (int i = 0; i < LEVELS; i++)
    {
        sum += (1 << i) * cnt[i];
    }
    assert(sum == pages_created);
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
        if (descs[bud_desc_num].state == BUDDY_FREE && descs[bud_desc_num].level == level)
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
    if (phys_addr % PAGE_SIZE != 0)
    {
        halt_program("Releasing address which is not dividable by the page size.");
    }
    int32_t desc_number = phys_addr / PAGE_SIZE;
    if (descs[desc_number].state != BUDDY_USED)
    {
        halt_program("Releasing not used memory.");
    }
    descs[desc_number].state = BUDDY_FREE;
    buddy_add_and_merge(desc_number);
}

