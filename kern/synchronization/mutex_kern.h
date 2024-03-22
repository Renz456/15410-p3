/**
 * @file mutex_kern.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-03-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _MUTEX_KERN_H
#define _MUTEX_KERN_H

#include <synchronization/list_helper.h>

typedef struct mutex
{
    int locked;
    int tid;
    mut_queue_t wait_queue;
} mutex_t;

int mutex_init(mutex_t *mp);
int mutex_lock(int tid, mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);
int mutex_destroy(mutex_t *mutex);

#endif /* _MUTEX_KERN_H */