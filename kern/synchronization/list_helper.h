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
    int reject;
    struct mut_node_s *next;
    struct mut_node_s *prev;
} mut_node_t;

typedef struct mut_queue_s
{
    mut_node_t *head;
    mut_node_t *tail;
    int size;
} mut_queue_t;

int queue_init(mut_queue_t *queue);

int queue_destroy(mut_queue_t *queue);

int add_queue(mut_node_t *new_node, mut_queue_t *queue);

mut_node_t *remove_queue(mut_queue_t *queue);

int queue_is_empty(mut_queue_t *queue);

#endif /* _LIST_HELPER_H */