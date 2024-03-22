/**
 * @file scheduler.c
 * @brief
 *
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 *
 * @bug Need to implement
 */

// #include <inc/mutex.h>
#include <thread_obj.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <simics.h>
#include <cr.h>
#include <asm_helpers.h>
#include <inc/scheduler.h>
#include <assert.h>
#include <interrupt_defines.h>

int context_enable = 0;

/*
Keep a lock for enabling and disabling context switches

Keep some data structure to keep track of tasks/threads

*/

// thread_obj_t *current_thread; // might not need this
tcb_t *run_queue_head;
tcb_t *run_queue_tail;
tcb_t *sleep_queue_head;
tcb_t *sleep_queue_tail;
tcb_t *wait_queue_head;
tcb_t *wait_queue_tail;
// do I need a block queue???

void initialise_scheduler()
{
    context_enable = 1;
    run_queue_head = NULL;
    run_queue_tail = NULL;
    sleep_queue_head = NULL;
    sleep_queue_tail = NULL;
    wait_queue_head = NULL;
    wait_queue_tail = NULL;
    // current_thread = NULL;
    return;
}

void enable_contexts()
{
    context_enable = 1;
}

void disable_contexts()
{
    context_enable = 0;
}

int try_switch()
{
    return context_enable;
}

void context_tickback(unsigned int tick)
{
    return;
}

void add_to_run_queue(tcb_t *tcb, int is_new_thread)
{
    tcb->next = NULL;
    tcb->next = NULL;
    tcb->new_thread = is_new_thread;
    insert_thread(&run_queue_head, &run_queue_tail, tcb);
    lprintf("added thread %d to run queue\n", tcb->tid);
    assert(run_queue_head);
}

int kernel_yield(int tid)
{
    context_switch(tid);
    outb(INT_CTL_PORT, INT_ACK_CURRENT);

    /* DO WE NEED SPECIAL CASING? */
    // if (tid == -1)
    // {
    //     context_switch(tid);
    //     outb(INT_CTL_PORT, INT_ACK_CURRENT);
    // }
    // else
    // {
    // }
}

/**
 * @brief
 *
 * @param tid
 * @param reject
 * @return int
 */
int kernel_deschedule(int tid, int *reject)
{
    disable_interrupts();
    if (reject == NULL || *reject != 0)
    {
        return -1;
    }

    /* will need to add to blocked/ sleep queue and yield */
    tcb_t *tcb = get_tcb();
    tcb->is_runnable = 0;
    insert_thread(&wait_queue_head, &wait_queue_tail, tcb);
    enable_interrupts();
    int result = kernel_yield(-1);
    return result;
}

/**
 * @brief THIS IS NOT THREAD SAFE
 *
 * @param tid
 * @return int
 */
int kernel_make_runnable(int tid)
{
    tcb_t *tcb = find_thread(&wait_queue_head, &wait_queue_tail, tid);
    if (!tcb)
        return -1;

    add_to_run_queue(tcb, 0);
    return 0;
}

/**
 * @brief Switches by taking the first thread off the queue
 *        (for round robin), storing current thread info on stack
 *        and leading the new thread's esp changing page directories (if needed)
 *
 *        Note no changes to eip need to be made as all threads will switch
 *        in this function.
 *
 *
 * @param tid
 */
void context_switch(int tid)
{
    if (context_enable == 0)
        return;
    /*
    Probably keep a list of tcbs or smth

    check if current tcb is the same as new tcb

    set cr3 to new page directory
    set kernel stack pointer (esp0)

    save user regs pusha + eip + flags + cr2 (really?) + cr3 (really?)
    what about code segments?? especailly if a thread is in kernel mode

    we can do set_cr3 and then pusha etc. because kernel stack is always in kernel space
    Consider the new thread case as it won't have stuff on its stack for registers
    */

    // Need to change tid logic before uncommenting these lines
    // tcb_t *current_thread = find_thread(run_queue_head, run_queue_tail, tid);
    tcb_t *cur_tcb = get_tcb();
    // assert(!current_thread);

    // add current thread to runnable queue and remove head of queue
    // (since round robin) as the next thread to execute.
    if (tcb->is_runnable)
        insert_thread(&run_queue_head, &run_queue_tail, cur_tcb);

    tcb_t *to_switch = find_thread(&run_queue_head, &run_queue_tail, tid);
    if (!to_switch)
    {
        to_switch = run_queue_head;
    }
    remove_thread(&run_queue_head, &run_queue_tail, to_switch);

    // void *save_stack = cur_tcb->esp;

    assert(to_switch->pcb->page_directory);

    if (cur_tcb->pcb->pid == to_switch->pcb->pid && cur_tcb->tid == to_switch->tid)
    {
        lprintf("thread %d will not switch\n", cur_tcb->tid);
        return;
    }
    set_cr3((uint32_t)to_switch->pcb->page_directory);
    set_esp0((uint32_t)to_switch->kernel_stack); // should this be tcb->esp instead?

    lprintf("switching to new thread %d pid %d from %d pid %d! \n", to_switch->tid, to_switch->pcb->pid, cur_tcb->tid, cur_tcb->pcb->pid);
    if (to_switch->new_thread)
    {
        to_switch->new_thread = 0; // set thread to old
        new_switch(&cur_tcb->esp, to_switch->esp);
    }
    else
    {
        finish_switch(&cur_tcb->esp, to_switch->esp);
    }
}