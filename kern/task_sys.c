/**
 * @file task_sys.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug NEED TO IMPLEMENT
 *
 */

#include <x86/cr.h>
#include <inc/task.h>
#include <inc/thread.h>
#include <inc/scheduler.h>
#include <inc/task_sys.h>
#include <inc/kern_constants.h>
#include <stdio.h>
#include <string.h>

void *clone_page_directory(void *pd)
{
    return NULL;
}

void set_child_stack(tcb_t *tcb, gen_reg_t *regs, tcb_t *cur_tcb)
{
    void *old_esp = regs - PTR_SIZE;
    unsigned int stack_offset = (unsigned int)cur_tcb->kernel_stack - ((unsigned int)old_esp);
    tcb->esp = tcb->kernel_stack - stack_offset;
    memcpy(tcb->esp, (void *)old_esp, stack_offset);
    gen_reg_t *new_regs = tcb->esp - sizeof(gen_reg_t);
    new_regs->edi = regs->edi;
    new_regs->esi = regs->esi;
    new_regs->ebp = regs->ebp;
    new_regs->zero = regs->zero;
    new_regs->ebx = regs->ebx;
    new_regs->edx = regs->edx;
    new_regs->ecx = regs->ecx;
    new_regs->eax = 0; // child should return 0 from fork
    tcb->esp = (void *)new_regs;
}

int kernel_fork(gen_reg_t *regs)
{
    /*
    Assign a new task id

    Deep copy of parent address space
    Do I need to copy all the threads its running too?
    should we not allow process to fork if it has more than one thread??
    Create kernel

    */
    void *cur_pd = (void *)get_cr3();

    void *new_pd = clone_page_directory(cur_pd);
    pcb_t *new_pcb = create_pcb(new_pd);
    // void *stack = init_task(new_pcb); // I should not need to do this
    tcb_t *new_tcb = create_tcb(new_pcb); // I need this since there's a new kernel stack

    /*
    Set the new threads registers etc. on the stack
    - somehow get current threads pusha, copy that onto the
      new threads kernel stack, set eax to 0?

    Add this to a runnnable queue
    */
    set_child_stack(new_tcb, regs, get_tcb());
    add_to_run_queue(new_tcb, 0);
    return new_pcb->pid;
}