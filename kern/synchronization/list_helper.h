/**
 * @file vm.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _LIST_HELPER_H
#define _LIST_HELPER_H

typedef struct mut_node_s 
{
    int tid;
    mut_node_s* next;
    mut_node_s* prev;
}mut_node_t;

typedef struct mut_queue_t
{
    mut_node_t* head;
    mut_node_t* tail;
    int size;
}mut_queue_t;

typedef struct mutex
{
    int locked;
    mut_queue_t wait_queue;
}mutex_t;

int queue_init(mut_queue_t* queue);

int queue_destroy(mut_queue_t* queue);

int add_queue(int tid, mut_queue_t* queue);

int remove_queue(mut_queue_t* queue);

#endif /* _LIST_HELPER_H */