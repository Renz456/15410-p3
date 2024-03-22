#include <simics.h>
#include <assert.h>
#include <stdlib.h>
#include <list_helper.h>
#include <mutex_kern.h>
#include <scheduler.h>

int mutex_init(mutex_t *mp)
{
    if (mp == NULL)
        return -1;
    mp->locked = 1;
    if (queue_init(&mp->wait_queue) < 0)
    {
        lprintf("Could not initialize mutex wait queue");
        return NULL;
    }
    return mp;
}

int mutex_lock(int tid, mutex_t *mutex)
{
    assert(mutex != NULL);
    // Stop context switching
    disable_contexts(); // or interupts??
    if (mutex->locked)
    {
        mutex->locked--;
        mutex->tid = tid;
        enable_contexts();
        // enable context switching
    }
    else
    {
        // TODO malloc is expensive can change
        mut_node_t node = {
            .tid = tid,
            .reject = 0,
            .next = NULL,
            .prev = NULL,
        };

        add_queue(&node, &mutex->wait_queue);

        enable_contexts();
        kernel_deschedule(tid, &node.reject);
        disable_contexts();

        mutex->locked--;
        mutex->tid = tid;
        enable_contexts();
    }
    return 0;
}

int mutex_unlock(mutex_t *mutex)
{
    assert(mutex != NULL);
    // stop context switching
    disable_contexts();
    if (queue_is_empty(&mutex->wait_queue))
    {
        mutex->locked++;
        // resume context switching
        enable_contexts();
    }
    else
    {
        mut_node_t *wait_node = remove_queue(&mutex->wait_queue);
        wait_node->reject = 1;
        kernel_make_runnable(wait_node->tid);
        mutex->locked++;
        mutex->tid = -1;
    }
    enable_contexts();
    return 0;
}

int mutex_destroy(mutex_t *mutex)
{
    assert(mutex != NULL && queue_is_empty(&mutex->wait_queue));
    int err_val = queue_destroy(&mutex->wait_queue);
    free(mutex);
    return err_val;
}
