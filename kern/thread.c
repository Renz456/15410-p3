/**
 * @file thread.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <inc/thread.h>

/**
 * @brief
 *
 * @return int
 */
int kernel_gettid()
{
    tcb_t *tcb = get_tcb();
    return tcb->tid;
}