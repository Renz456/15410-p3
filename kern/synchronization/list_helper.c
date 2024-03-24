#include <stdlib.h>
#include <synchronization/list_helper.h>
#include <assert.h>

int queue_init(mut_queue_t *queue)
{
    if (queue == NULL)
    {
        return -1;
    }
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return 0;
}

int queue_destroy(mut_queue_t *queue)
{
    return 0;
}

int add_queue(mut_node_t *new_node, mut_queue_t *queue)
{
    assert(queue != NULL);

    if (queue->size == 0)
    {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else
    {
        assert(queue->size > 0);
        new_node->prev = queue->tail;
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size += 1;
    return 0;
}

mut_node_t *remove_queue(mut_queue_t *queue)
{
    assert(queue != NULL && queue->size > 0);
    mut_node_t *free_node = queue->head;
    queue->head = queue->head->next;
    if (queue->head != NULL)
    {
        queue->head->prev = NULL;
    }
    queue->size -= 1;

    return free_node;
}

int queue_is_empty(mut_queue_t *queue)
{
    assert(queue != NULL);
    return queue->size == 0;
}