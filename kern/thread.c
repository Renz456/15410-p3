/**
 * @file thread.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <thread.h>
#include <inc/asm_helpers.h>
#include <inc/kern_constants.h>
#include <stdint.h>
#include <stdlib.h>
#include <seg.h>
#include <x86/cr.h>
#include <x86/eflags.h>
#include <simics.h>
#include <assert.h>
#include <vm.h>
#include <scheduler.h>

int num_threads = 0;

/**
 * @brief Create a tcb object
 *
 * @param tid
 * @param stack
 * @param pcb
 * @return tcb_t*
 */
tcb_t *create_tcb(pcb_t *pcb)
{

    void *kernel_stack = smemalign(KERNEL_PAGE_SIZE, KERNEL_PAGE_SIZE);
    if (!kernel_stack)
        return NULL;

    tcb_t *tcb = malloc(sizeof(tcb_t));
    if (!tcb)
        return NULL;

    void *kernel_stack_high = kernel_stack + KERNEL_PAGE_SIZE;
    *((unsigned int *)(kernel_stack_high - PTR_SIZE)) = (unsigned int)tcb; // set tcb to the top of kernel stack

    tcb->tid = atomic_increment(&num_threads);
    atomic_increment(&pcb->num_threads);

    tcb->kernel_stack = kernel_stack_high - 2 * PTR_SIZE;
    tcb->pcb = pcb;
    tcb->new_thread = 0;
    tcb->is_runnable = 1;
    // tcb->stack_high = stack;
    // tcb->stack_low = stack - KERNEL_PAGE_SIZE;

    return tcb;
}

/**
 * @brief Get the tcb object
 *
 * @return tcb_t*
 */
tcb_t *get_tcb()
{
    void *stack = get_esp();
    void *stack_bottom = (void *)((uintptr_t)stack & KERNEL_PAGE_MASK);
    tcb_t **tcb_ptr = (tcb_t **)(stack_bottom + KERNEL_PAGE_SIZE - PTR_SIZE);
    return *tcb_ptr;
}

usr_state_t set_user_state(tcb_t *tcb, void *stack_base, void *entry_instruction)
{
    usr_state_t user_state = {
        .eip = (uint32_t)entry_instruction,
        .cs = SEGSEL_USER_CS,
        .eflags = get_eflags() | EFL_IF | EFL_RESV1,
        .esp = (uint32_t)stack_base,
        .ss = SEGSEL_USER_DS,
    };

    return user_state;
}

void prepare_thread(tcb_t *tcb, void *stack_base, void *entry_instruction)
{
    usr_state_t state = set_user_state(tcb, stack_base, entry_instruction);
    usr_state_t *load_user = (usr_state_t *)(stack_base - sizeof(usr_state_t));
    *load_user = state;
    tcb->esp = (void *)load_user;
    usr_state_t user_state = *load_user;
    lprintf("SS: %lx\n", user_state.ss);
    lprintf("esp: %lx\n", user_state.esp);
    lprintf("eflags: %lx\n", user_state.eflags);
    lprintf("cs: %lx\n", user_state.cs);
    lprintf("eip: %lx\n", user_state.eip);
    add_to_run_queue(tcb, 1);
}

void run_thread(tcb_t *tcb, void *stack_base, void *entry_instruciton)
{
    // what needs to be here lol
    // mark pointer to kernel stack for future interupts
    assert(tcb);
    set_esp0((uint32_t)tcb->kernel_stack);
    tcb->esp = stack_base;
    usr_state_t user_state = set_user_state(tcb, stack_base, entry_instruciton);
    lprintf("check user state before run eip: %lx esp:%lx\n", user_state.eip, user_state.esp);
    // user_state.cs += 1;
    // lprintf("SS: %lx\n", user_state.ss);
    // lprintf("esp: %lx\n", user_state.esp);
    // lprintf("eflags: %lx\n", user_state.eflags);
    // lprintf("cs: %lx\n", user_state.cs);
    // lprintf("eip: %lx\n", user_state.eip);
    // add_to_run_queue(tcb, 0);
    MAGIC_BREAK;
    exec_user(user_state);
}

/**
 * @briefsc
 *
 * @return int
 */
int kernel_gettid()
{
    tcb_t *tcb = get_tcb();
    return tcb->tid;
}
