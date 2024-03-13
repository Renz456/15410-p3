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

int context_enable = 0;

/*
Keep a lock for enabling and disabling context switches

Keep some data structure to keep track of tasks/threads

*/

// thread_obj_t *current_thread; // might not need this
thread_obj_t *run_queue_head;
thread_obj_t *run_queue_tail;
thread_obj_t *sleep_queue_head;
thread_obj_t *sleep_queue_tail;
// do I need a block queue???

void initialise_scheduler()
{
    context_enable = 1;
    run_queue_head = NULL;
    run_queue_tail = NULL;
    sleep_queue_head = NULL;
    sleep_queue_tail = NULL;
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
    thread_obj_t *new_thread = malloc(sizeof(thread_obj_t));
    new_thread->tcb = tcb;
    new_thread->tid = tcb->tid;
    new_thread->next = NULL;
    new_thread->prev = NULL;
    new_thread->new_thread = is_new_thread;
    insert_thread(&run_queue_head, &run_queue_tail, new_thread);
    lprintf("added thread %d to run queue\n", tcb->tid);
    assert(run_queue_head);
}

void context_switch(int tid)
{
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
    // if (tid == -1)
    // {
    //     tid = kernel_gettid();
    // }

    // MAGIC_BREAK;

    thread_obj_t *current_thread = find_thread(run_queue_head, run_queue_tail, tid);
    tcb_t *cur_tcb;
    if (current_thread)
    {
        remove_thread(&run_queue_head, &run_queue_tail, current_thread);
        insert_thread(&run_queue_head, &run_queue_tail, current_thread);
        find_thread(run_queue_head, run_queue_tail, 2);
        cur_tcb = current_thread->tcb;
        assert(run_queue_tail->tid == current_thread->tid);
    }
    else
    {
        lprintf("Thread not in run queue\n");
        cur_tcb = get_tcb();
        // return;
    }

    thread_obj_t *to_switch = run_queue_head;

    // void *save_stack = cur_tcb->esp;

    assert(to_switch->tcb->pcb->page_directory);

    if (cur_tcb->tid == to_switch->tid)
    {
        lprintf("thread %d will not switch\n", cur_tcb->tid);
        return;
    }

    set_cr3((uint32_t)to_switch->tcb->pcb->page_directory);
    set_esp0((uint32_t)to_switch->tcb->kernel_stack); // should this be tcb->esp instead?

    lprintf("switching to new thread %d from %d! \n", to_switch->tid, cur_tcb->tid);
    if (to_switch->new_thread)
    {
        to_switch->new_thread = 0; // set thread to old
        new_switch(&cur_tcb->esp, to_switch->tcb->esp);
    }
    else
    {
        finish_switch(&cur_tcb->esp, to_switch->tcb->esp);
    }
}