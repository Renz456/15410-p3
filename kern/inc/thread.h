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
#include <stdint.h>

typedef struct tcb
{
    int tid;
    // void *stack_high;
    // void *stack_low;
    void *kernel_stack;
    // void *esp;
    // void *eip;
    pcb_t *pcb;
    // registers?
    // process pointer?
    // kernel info?
} tcb_t;

typedef struct usr_state
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} usr_state_t;