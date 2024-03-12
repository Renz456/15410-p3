/**
 * @file mutex.c
 * @author Rene Ravanan (rravanan)
 * @brief
 *
 * @bug Consider ticket based for fairness
 *
 *
 */

#include <stdlib.h>
#include <mutex_type.h>
#include <syscall.h>
#include <xchg.h>
#include <assert.h>
#include <simics.h>

/** @brief Initializes the mutex by setting the internal struct
 *         values to their default vals
 *  @param mp The mutex to the initialized
 *  @return 0 on success, -1 on failure
 */
int mutex_init(mutex_t *mp)
{
    if (mp == NULL)
        return -1;

    mp->lock = 1;
    mp->tid = -1;
    mp->valid = 1;
    return 0;
}

/** @brief Destroying a mutex by setting it to invalid
 *  @param mp The mutex to be destroyed
 */
void mutex_destroy(mutex_t *mp)
{
    affirm(mp != NULL);
    affirm(mp->valid);
    affirm(mp->lock == 1);
    // lprintf("TID: %d, valid: %d, lock: %d\n", mp->tid, mp->valid, mp->lock);
    mp->valid = 0;
    mp->tid = -1;
    // MAGIC_BREAK;
    return;
}

/** @brief Tries to lock a mutex by doing busy waiting
 *         and continuially checking if the lock is free or not
 *  @param mp mutex which is to be locked
 */
void mutex_lock(mutex_t *mp)
{
    affirm(mp != NULL && mp->valid);
    while (!xchg_wrapper(&mp->lock, 0)) // Check if the lock is free
    {
        yield(mp->tid); // yield to lock holder as it is still not free
    }
    mp->tid = gettid();
}

/** @brief Unlock the mutex by simply setting the lock variable in the struct
 *  @param mp - Mutex to be unlocked
 */
void mutex_unlock(mutex_t *mp)
{
    affirm(mp != NULL);
    affirm(mp->valid);
    affirm(mp->lock == 0);
    mp->lock = 1;
    mp->tid = -1;
    return;
}