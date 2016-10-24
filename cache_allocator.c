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

struct cache_allocator *cache_allocator_init(uint16_t size)
{
    if (size < NODE_SIZE)
    {
        size = NODE_SIZE;
    }
    int level = size <= SMALL ? SMALL_LEVEL : LARGE_LEVEL;
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

void cache_allocator_deinit(struct cache_allocator *allocator)
{
    allocator->head = NOTHING;
    buddy_release(allocator); 
}

void *cache_allocator_alloc(struct cache_allocator *allocator)
{
    struct cache_node_info *result = allocator->head;
    if (result != NOTHING)
    {
        allocator->head = result->next;
    }
    return result;
}

void cache_allocator_free(struct cache_allocator *allocator, void *ptr)
{
    struct cache_node_info *node = (struct cache_node_info *) ptr;
    struct cache_node_info *head = allocator->head;
    node->next = allocator->head;
    allocator->head = node;
}

