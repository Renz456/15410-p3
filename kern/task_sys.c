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
#include <inc/vm.h>
#include <simics.h>
#include <stdio.h>
#include <string.h>
#include <inc/asm_helpers.h>

void set_child_stack(tcb_t *tcb, gen_reg_t *regs, tcb_t *cur_tcb, void *esp)
{
    /* esp i want : 0x00148fa4*/
    /* eip i want: bc21 1000 = 001021bc */
    // TODO explain this magic number
    void *old_esp = (void *)((uint32_t)regs - 6 * PTR_SIZE);
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
    lprintf("old esp %p regs ardd %p and new esp %p\n", old_esp, regs, (void *)(*((unsigned int *)tcb->esp)));
    MAGIC_BREAK;
    tcb->esp = (void *)new_regs;
    lprintf("edi %x\n", new_regs->edi);
    lprintf("esi %x\n", new_regs->esi);
    lprintf("ebp %x\n", new_regs->ebp);
    lprintf("zero %x\n", new_regs->zero);
    lprintf("ebx %x\n", new_regs->ebx);
    lprintf("edx %x\n", new_regs->edx);
    lprintf("ecx %x\n", new_regs->ecx);
    lprintf("eax %x\n", new_regs->eax);
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

    // 3c1f 1500
    // MAGIC_BREAK;
    void *old_esp = get_esp();
    lprintf("what esp is this %p\n", old_esp);
    void *cur_pd = (void *)get_cr3();

    void *new_pd = clone_page_directory(cur_pd);
    lprintf("Parent successful clone\n");
    pcb_t *new_pcb = create_pcb(new_pd);
    // void *stack = init_task(new_pcb); // I should not need to do this
    tcb_t *new_tcb = create_tcb(new_pcb); // I need this since there's a new kernel stack

    /*
    Set the new threads registers etc. on the stack
    - somehow get current threads pusha, copy that onto the
      new threads kernel stack, set eax to 0?

    Add this to a runnnable queue
    */
    set_child_stack(new_tcb, regs, get_tcb(), old_esp);
    add_to_run_queue(new_tcb, 0);

    lprintf("Parent exiting syscall\n");
    return new_pcb->pid;
}