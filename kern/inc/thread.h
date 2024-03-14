/**
 * @file thread.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _THREAD_KERN_H
#define _THREAD_KERN_H

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
    void *esp;

    struct tcb *next;
    struct tcb *prev;
    int new_thread;
} tcb_t;

typedef struct usr_state
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} usr_state_t;

void run_thread(tcb_t *tcb, void *stack_base, void *entry_instruction);
tcb_t *get_tcb();
tcb_t *create_tcb(pcb_t *pcb);
void prepare_thread(tcb_t *tcb, void *stack_base, void *entry_instruction);

#endif /* _THREAD_KERN_H */