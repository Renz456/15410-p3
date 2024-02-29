/**
 * @file thread.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <task.h>

typedef struct tcb
{
    int tid;
    void *stack_hi;
    void *stack_low;
    void *esp;
    void *eip;
    pcb_t *pcb;
    // registers?
    // process pointer?
    // kernel info?
} tcb_t;