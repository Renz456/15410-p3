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

void insert_thread(thread_obj_t *head, thread_obj_t *tail, thread_obj_t *thread)
{
    thread->next = NULL;
    thread->prev = tail;
    tail->next = thread;
    tail = thread;
}

void remove_thread(thread_obj_t *head, thread_obj_t *tail, thread_obj_t *thread)
{
    head = head->next;
    head->prev = NULL;
}

thread_obj_t *find_thread(thread_obj_t *head, thread_obj_t *tail, int tid)
{
    for(thread_obj_t *start = head; start != NULL; start = start->next){
        if(start->tid == tid){
            return start;
        }
    }
    return NULL;
}
