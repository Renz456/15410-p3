/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

typedef struct Node
{
    int tid;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct Queue
{
    Node *Head;
    Node *Tail;
    int size; // not really necessary tbh
} Queue;

typedef struct cond
{
    Queue *wait_queue;
    mutex_t mutex;
    int tid;
    int valid;
} cond_t;

int cond_init(cond_t *cv);

void cond_destroy(cond_t *cv);

void cond_wait(cond_t *cv, mutex_t *mp);

void cond_signal(cond_t *cv);

void cond_broadcast(cond_t *cv);

#endif /* _COND_TYPE_H */
