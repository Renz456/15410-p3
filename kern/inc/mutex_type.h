/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct mutex
{
  int tid;
  int lock;
  int valid;
} mutex_t;

int mutex_init(mutex_t *mp);

void mutex_destroy(mutex_t *mp);

void mutex_lock(mutex_t *mp);

void mutex_unlock(mutex_t *mp);

#endif /* _MUTEX_TYPE_H */
