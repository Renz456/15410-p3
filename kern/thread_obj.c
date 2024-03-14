/**
 * @file thread_obj.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <thread_obj.h>
#include <stdio.h>
#include <assert.h>

void insert_thread(tcb_t **head, tcb_t **tail, tcb_t *thread)
{
    assert(thread != NULL);
    thread->next = NULL;
    thread->prev = *tail;
    if (*tail == NULL)
    {
        *head = thread;
        *tail = thread;
        return;
    }
    (*tail)->next = thread;
    *tail = thread;
}

void remove_thread(tcb_t **head, tcb_t **tail, tcb_t *thread)
{
    if (thread->next == NULL && thread->prev == NULL)
    { // Only thread in the queue
        *head = NULL;
        *tail = NULL;
    }
    else if (thread->next == NULL)
    { // Last thing in the queue
        (*tail) = (*tail)->prev;
        (*tail)->next = NULL;
    }
    else if (thread->prev == NULL)
    { // First thing in the queue
        (*head) = (*head)->next;
        (*head)->prev = NULL;
    }
    else
    { // Somewhere in the middle
        thread->prev->next = thread->next;
        thread->next->prev = thread->prev;
    }
}

tcb_t *find_thread(tcb_t *head, tcb_t *tail, int tid)
{
    for (tcb_t *start = head; start != NULL; start = start->next)
    {
        if (start->tid == tid)
        {
            return start;
        }
    }
    return NULL;
}
