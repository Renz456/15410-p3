/**
 * @file thread_obj.h
 * @author Rene Ravanan(rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _THREAD_OBJ_H
#define _THREAD_OBJ_H

#include <thread.h>

typedef struct thread_obj
{
    int tid;
    int new_thread;
    tcb_t *tcb;
    struct thread_obj *next;
    struct thread_obj *prev;

} thread_obj_t;

void insert_thread(thread_obj_t **head, thread_obj_t **tail, thread_obj_t *thread);
void remove_thread(thread_obj_t **head, thread_obj_t **tail, thread_obj_t *thread);
thread_obj_t *find_thread(thread_obj_t *head, thread_obj_t *tail, int tid);

#endif /* _THREAD_OBJ_H */