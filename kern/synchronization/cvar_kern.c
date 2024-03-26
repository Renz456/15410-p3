/**
 * @file cvar.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief Contains implementation for the cvar library functions defined in cond_types.h
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <simics.h>
#include <assert.h>
#include <asm.h>
#include <synchronization/mutex_kern.h>
#include <synchronization/list_helper.h>
#include <synchronization/cvar_kern.h>
#include <scheduler.h>
#include <inc/thread.h>

/** @brief Initializes the condition variable
 *  @param cv - pointer to the condition variable
 *  @return 0 on success, -1 on error
 */
int cond_init(cond_t *cv)
{
    if (cv == NULL)
        return -1;
    cv->wait_queue = malloc(sizeof(mut_queue_t));
    if (cv->wait_queue == NULL)
    {
        return -1;
    }
    queue_init(cv->wait_queue);
    if (mutex_init(&cv->mutex))
    {
        return -1;
    }
    cv->valid = 1;
    return 0;
}

/** @brief Destroying cond var by free its queue and destroying its mutex
 *  @param cv The cond var to be destroyed
 */
void cond_destroy(cond_t *cv)
{
    affirm(cv != NULL && cv->valid);
    if (queue_is_empty(cv->wait_queue))
    {
        free(cv->wait_queue);
        mutex_destroy(&cv->mutex);
        cv->valid = 0;
        return;
    }
    panic("Trying to destroy a cond variable which threads are waiting on");
}

/** @brief Waiting on a mutex using a condition variable
 *  @param cv the condition variable
 *  @param mp mutex which will be regained after being signalled
 */
void cond_wait(cond_t *cv, mutex_t *mp)
{
    affirm(cv != NULL && mp != NULL && cv->valid);
    if (!cv->valid)
        return;
    int tid = kernel_gettid();
    mut_node_t node = {
        .tid = tid,
        .reject = 0,
        .next = NULL,
        .prev = NULL,
    };
    mutex_lock(tid, &cv->mutex);
    add_queue(&node, cv->wait_queue); // Putting ourselves on the wait queue
    mutex_unlock(mp);
    lprintf("cehck lock status %d\n", mp->locked);
    mutex_unlock(&cv->mutex);
    kernel_deschedule(tid, &node.reject); // Should we check for the ret val of this
    // lprintf("cond wait spinning\n");
    // while (!node.reject)
    // {
    // }
    mutex_lock(tid, mp);
}

/** @brief Signalling the next waiting thread that the lock is now available
 *  @param cv The condition variable
 */
void cond_signal(cond_t *cv)
{
    affirm(cv != NULL);
    mutex_lock(kernel_gettid(), &cv->mutex);
    if (!queue_is_empty(cv->wait_queue))
    {
        mut_node_t *waiter_node = remove_queue(cv->wait_queue);
        waiter_node->reject = 1;
        mutex_unlock(&cv->mutex);
        disable_interrupts();
        // lprintf("")
        kernel_make_runnable(waiter_node->tid);
        enable_interrupts();
    }
    else
    {
        mutex_unlock(&cv->mutex);
    }
}

/** @brief Make all the waiting threads runnable
 *  @param cv condition variable for which the threads are waiting
 */
void cond_broadcast(cond_t *cv)
{
    affirm(cv != NULL && cv->valid);
    mutex_lock(kernel_gettid(), &cv->mutex);
    while (!queue_is_empty(cv->wait_queue))
    {
        mut_node_t *waiter_node = remove_queue(cv->wait_queue);
        waiter_node->reject = 1;
        mutex_unlock(&cv->mutex);
        kernel_make_runnable(waiter_node->tid);
        mutex_lock(kernel_gettid(), &cv->mutex);
    }
    mutex_unlock(&cv->mutex);
}
