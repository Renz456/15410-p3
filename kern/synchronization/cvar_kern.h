/**
 * @file cvar_kern.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-03-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _CVAR_KERN_H
#define _CVAR_KERN_H

#include <synchronization/mutex_kern.h>
#include <synchronization/list_helper.h>

/**
 * @brief
 *
 */
typedef struct cond
{
    int valid;
    int locked;
    int tid;
    mutex_t mutex;
    mut_queue_t *wait_queue;
} cond_t;

int cond_init(cond_t *cv);
void cond_destroy(cond_t *cv);
void cond_wait(cond_t *cv, mutex_t *mp);
void cond_signal(cond_t *cv);
void cond_broadcast(cond_t *cv);

#endif /* _CVAR_KERN_H */