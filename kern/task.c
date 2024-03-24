/**
 * @file task.c
 * @author your name (you@domain.com)
 * @brief
 *
 * @bug
 *
 */

#include <inc/task.h>
#include <inc/thread.h>
#include <inc/kern_constants.h>
#include <inc/scheduler.h>
#include <inc/asm_helpers.h>
#include <malloc.h>
#include <assert.h>
#include <simics.h>
#include <interrupt_defines.h>
#include <asm.h>
#include <synchronization/mutex_kern.h>
#include <synchronization/cvar_kern.h>
// #include <vm.h>

#define TASK_PAGE_SIZE PAGE_SIZE
int next_pid = 0;
pcb_t *main_pcb;

static void *setup_main(void *stack_high, void *stack_low, char **argv, int argc);

pcb_t *create_pcb(void *page_directory)
{
    pcb_t *pcb = malloc(sizeof(pcb_t));
    assert(pcb);
    pcb->page_directory = page_directory;
    pcb->num_threads = 0;
    void *stack_low = (void *)((USER_MEM_END & PAGE_MASK) - TASK_PAGE_SIZE);
    void *stack_high = (void *)(USER_MEM_END & PAGE_MASK);
    pcb->stack_high = stack_high;
    pcb->stack_low = stack_low;
    pcb->pid = next_pid++;
    pcb->parent = NULL;
    pcb->child_list = NULL;
    pcb->next = NULL;
    pcb->prev = NULL;
    if (mutex_init(&pcb->pcb_mp) < 0 || cond_init(&pcb->pcb_cv) < 0)
    {
        free(pcb);
        return NULL;
    }
    return pcb;
}

/**
 * @brief Returns top of user stack for program?
 *
 * @param pcb : This might not be necessary
 * @return void*
 */
void *init_task(pcb_t *pcb, char **argv, int argc)
{
    void *stack_low = pcb->stack_low;
    void *stack_high = stack_low + TASK_PAGE_SIZE;
    pcb->stack_low -= TASK_PAGE_SIZE;
    if (new_pages(stack_low, TASK_PAGE_SIZE) < 0)
        return NULL;

    lprintf("check start %p end %p stacks for procss\n", stack_high, stack_low);
    return setup_main(stack_high, stack_low, argv, argc);
}

/**
 * @brief Set the up main object
 *
 * @param stack_high
 * @param stack_low
 * @return void*
 */
static void *setup_main(void *stack_high, void *stack_low, char **argv, int argc)
{
    crt0_main_t *args = stack_high - sizeof(crt0_main_t);
    args->argc = argc;
    args->argv = argv;
    args->stack_high = stack_high;
    args->stack_low = stack_low;

    return stack_high - sizeof(crt0_main_t) - PTR_SIZE;
}

void add_child(pcb_t **parent_list, pcb_t *child)
{
    assert(parent_list && child);
    if (*parent_list == NULL)
    {
        *parent_list = child;
        child->next = NULL;
    }
    else
    {
        (*parent_list)->prev = child;
        child->next = *parent_list;
        *parent_list = child;
    }
    child->prev = NULL;
}

void remove_child(pcb_t **parent_list, pcb_t *child)
{
    assert(parent_list && child);
    if (child->next == NULL && child->prev == NULL)
    {
        *parent_list = NULL;
    }
    else if (child->next == NULL)
    {
        (*parent_list) = (*parent_list)->prev;
        (*parent_list)->next = NULL;
    }
    else if (child->prev == NULL)
    {
        (*parent_list) = (*parent_list)->next;
        (*parent_list)->prev = NULL;
    }
    else
    {
        child->prev->next = child->next;
        child->next->prev = child->prev;
    }
    child->next = NULL;
    child->prev = NULL;
}

void kernel_task_vanish(int status)
{
    /* this will definitely be a deadlock/race lol*/
    /* To avoid deadlock keep preemption by always locking child first?*/
    tcb_t *tcb = get_tcb();
    pcb_t *pcb = tcb->pcb;

    /* let children know */
    for (pcb_t *child = pcb->child_list; child != NULL; child = child->next)
    {
        // TODO check this input
        mutex_lock(child->pid, &child->pcb_mp);
        child->parent = NULL;
        mutex_unlock(&child->pcb_mp);
    }

    /* TODO add zombies to main pcb too */

    mutex_lock(tcb->tid, &pcb->pcb_mp);
    pcb_t *parent = pcb->parent;
    if (!parent) // shouldn't happen for now
        parent = main_pcb;

    mutex_lock(tcb->tid, &parent->pcb_mp);
    remove_child(&parent->child_list, pcb);
    pcb->exited = 1; // this will be status later
    pcb->status = status;
    add_child(&parent->zombie_list, pcb);

    mutex_unlock(&parent->pcb_mp);
    mutex_unlock(&pcb->pcb_mp);

    /* clear user vm here? */

    disable_interrupts();
    tcb->is_runnable = 0;
    context_switch(-1);

    /* should not return back here */
    assert(1 == 2);
}

void kernel_vanish()
{
    tcb_t *tcb = get_tcb();
    pcb_t *pcb = tcb->pcb;

    atomic_decrement(&pcb->num_threads);
    /* These two should be atomic too, otherwise two threads can call task vanish*/
    if (pcb->num_threads == 0)
    {
        /* basically task vanish */
        kernel_task_vanish(1);
    }

    /* any vm clearing? */

    /* need to disable interupts to safely switch out of this
        thread but when to reenable*/

    disable_interrupts();
    tcb->is_runnable = 0;
    context_switch(-1);

    /* should not return back here */
    assert(3 == 4);
}

/* TODO will need to consider freeing tcb and maybe even pcb */
/* can't free in vanish and context switch needs them */