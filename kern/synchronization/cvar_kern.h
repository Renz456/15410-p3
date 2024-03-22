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

#include <mutex_kern.h>
#include <list_helper.h>

/**
 * @brief
 *
 */
typedef struct mutex
{
    int locked;
    int tid;
    mutex_t mutex;
    mut_queue_t *wait_queue;
} cond_t;