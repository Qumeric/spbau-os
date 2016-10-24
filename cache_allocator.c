#include <cache_allocator.h>
#include <buddy_allocator.h>
#include <memory.h>
#include <utils.h>

#define SMALL 512
#define SMALL_LEVEL 0
#define LARGE_LEVEL 2

#define NODE_SIZE sizeof(struct cache_node_info)
#define ALLC_SIZE sizeof(struct cache_allocator)

#define NOTHING ((void *) -1)

static int get_level(uint16_t size)
{
     return size <= SMALL ? SMALL_LEVEL : LARGE_LEVEL;   
}

struct cache_allocator *cache_allocator_init(uint16_t size)
{
    if (size < NODE_SIZE)
    {
        size = NODE_SIZE;
    }
    int level = get_level(size);
    void *initial = buddy_allocate(level);
    if (initial == NOTHING)
    {
        return NOTHING;
    }
    void *finish = ((char *) initial) + (PAGE_SIZE << level);

    struct cache_allocator *ptr = (struct cache_allocator *) initial;
    ptr->size = size;
    
    struct cache_node_info *last = &(ptr->head);
    struct cache_node_info *curr = (struct cache_node_info *) (ptr + 1);
    while (((char *) curr) + size <= finish)
    {
        curr->next = NOTHING;
        last->next = curr;
        last = curr;
        curr = ((char *) curr) + size;
    }
    return ptr;
}

void *cache_allocator_alloc(struct cache_allocator *allocator)
{
    if (allocator->head == NOTHING)
    {
        // allocator more space and attach to the allocator's list
        uint16_t size = allocator->size;
        int level = get_level(size);
        void *initial = buddy_allocate(level);
        if (initial == NOTHING)
        {
            return NOTHING;
        }
        void *finish = ((char *) initial) + (PAGE_SIZE << level);

        struct cache_node_info *last = &(allocator->head);
        struct cache_node_info *curr = (struct cache_node_info *) initial;
        while (((char *) curr) + size <= finish)
        {
            curr->next = NOTHING;
            last->next = curr;
            last = curr;
            curr = ((char *) curr) + size;
        }
    }

    struct cache_node_info *result = allocator->head;
    allocator->head = result->next;
    return result;
}

void cache_allocator_free(struct cache_allocator *allocator, void *ptr)
{
    struct cache_node_info *node = (struct cache_node_info *) ptr;
    struct cache_node_info *head = allocator->head;
    node->next = allocator->head;
    allocator->head = node;
}

