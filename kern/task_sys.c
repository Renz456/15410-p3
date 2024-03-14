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

int kernel_fork()
{
    /*
    Assign a new task id

    Deep copy of parent address space
    Do I need to copy all the threads its running too?

    Create kernel

    */
    void *cur_pd = get_cr3();

    void *new_pd = clone_page_directory(cur_pd);
    pcb_t *new_pcb = create_pcb(new_pd);
    void *stack = init_task(new_pcb);
    tcb_t *new_tcb = create_tcb(new_pcb);

    /*
    Set the new threads registers etc. on the stack
    Add this to a runnnable queue

    */
    add_to_run_queue(new_tcb, 0);
    return new_pcb->pid;
}