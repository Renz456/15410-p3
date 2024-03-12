/**
 * @file cvar.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief Contains implementation for the cvar library functions defined in cond_types.h
 *
 */

#include <mutex_type.h>
#include <stdio.h>
#include <malloc.h>
#include <syscall.h>
#include <cond_type.h>
#include <simics.h>
#include <assert.h>

/** @brief Initializes the wait queue for cond vars
 * @param wait_queue Queue which is to be initialized
 */
static void initialize_queue(Queue *wait_queue)
{
    affirm(wait_queue != NULL);
    wait_queue->Head = NULL;
    wait_queue->Tail = NULL;
    wait_queue->size = 0;
    return;
}

/** @brief Checks if the wait queue is empty
 * @param wait_queue The wait queue which is to be checked
 * @return 1 if empty,  0 otherwise
 */
static int is_empty(Queue *wait_queue)
{
    affirm(wait_queue != NULL);
    if (wait_queue->size == 0)
    {
        return 1;
    }
    return 0;
}

/** @brief Creates a node (to add into wait queue) for a waiting thread
 *  @param tid TID of waiting thread
 *  @return The node itself
 */
static Node *create_node(int tid)
{
    Node *ret_node = (Node *)malloc(sizeof(Node));
    affirm(ret_node != NULL);
    ret_node->tid = tid;
    ret_node->next = NULL;
    ret_node->prev = NULL;
    return ret_node;
}

/** @brief Appends a thread to the wait queue
 *  @param wait_queue The wait queue which is to be added
 *  @param tid The TID of the thread which is to be added to the
 *             wait queue
 */
static void append_node(Queue *wait_queue, int tid)
{
    affirm(wait_queue != NULL);
    Node *insert_node = create_node(tid);
    if (is_empty(wait_queue))
    {
        wait_queue->Head = insert_node;
        wait_queue->Tail = insert_node;
        wait_queue->size += 1;
        return;
    }
    Node *temp = wait_queue->Head;
    wait_queue->Head = insert_node;
    insert_node->next = temp;
    temp->prev = insert_node;
    wait_queue->size += 1;
    return;
}

/** @brief Removes the thread node which has been waiting the longest
 *  @param wait_queue The queue out of which node is to be removed
 *  @return TID of thread node which has been removed
 */
static int remove_node(Queue *wait_queue)
{
    affirm(wait_queue != NULL);
    int ret_val = wait_queue->Tail->tid;
    Node *temp = wait_queue->Tail;
    if (wait_queue->Head == temp)
    {
        wait_queue->Head = NULL;
    }
    wait_queue->Tail = temp->prev;
    free(temp);
    wait_queue->size -= 1;
    return ret_val;
}

/** @brief Initializes the condition variable
 *  @param cv - pointer to the condition variable
 *  @return 0 on success, -1 on error
 */
int cond_init(cond_t *cv)
{
    if (cv == NULL)
        return -1;
    cv->wait_queue = malloc(sizeof(Queue));
    if (cv->wait_queue == NULL)
    {
        return -1;
    }
    initialize_queue(cv->wait_queue);
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
    if (is_empty(cv->wait_queue))
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
    int tid = gettid();
    mutex_lock(&cv->mutex);
    append_node(cv->wait_queue, tid); // Putting ourselves on the wait queue
    mutex_unlock(mp);
    int reject = 0;
    mutex_unlock(&cv->mutex);
    deschedule(&reject); // Should we check for the ret val of this
    mutex_lock(mp);
}

/** @brief Signalling the next waiting thread that the lock is now available
 *  @param cv The condition variable
 */
void cond_signal(cond_t *cv)
{
    affirm(cv != NULL);
    mutex_lock(&cv->mutex);
    if (!is_empty(cv->wait_queue))
    {
        int wait_up_tip = remove_node(cv->wait_queue);
        mutex_unlock(&cv->mutex);
        while (make_runnable(wait_up_tip) < 0) // Discussed this further in the readme
        {
            yield(wait_up_tip);
        }
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
    mutex_lock(&cv->mutex);
    while (!is_empty(cv->wait_queue))
    {
        int wait_up_tip = remove_node(cv->wait_queue);
        mutex_unlock(&cv->mutex);
        while (make_runnable(wait_up_tip) < 0)
        {
            yield(wait_up_tip);
        }
        mutex_lock(&cv->mutex);
    }
    mutex_unlock(&cv->mutex);
}
