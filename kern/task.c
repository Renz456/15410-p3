/**
 * @file task.c
 * @author your name (you@domain.com)
 * @brief
 *
 * @bug
 *
 */

#include <inc/task.h>
#include <inc/kern_constants.h>
#include <malloc.h>
#include <assert.h>
#include <simics.h>
// #include <vm.h>

#define TASK_PAGE_SIZE PAGE_SIZE
int next_pid = 0;

static void *setup_main(void *stack_high, void *stack_low);

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
    return pcb;
}

/**
 * @brief
 *
 * @param pcb : This might not be necessary
 * @return void*
 */
void *init_task(pcb_t *pcb)
{
    void *stack_low = pcb->stack_low;
    void *stack_high = stack_low + TASK_PAGE_SIZE;
    pcb->stack_low -= TASK_PAGE_SIZE;
    if (new_pages(stack_low, TASK_PAGE_SIZE) < 0)
        return NULL;

    lprintf("check start %p end %p stacks for procss\n", stack_high, stack_low);
    return setup_main(stack_high, stack_low);
}

/**
 * @brief Set the up main object
 *
 * @param stack_high
 * @param stack_low
 * @return void*
 */
static void *setup_main(void *stack_high, void *stack_low)
{
    crt0_main_t *args = stack_high - sizeof(crt0_main_t);
    args->argc = 0;
    args->argv = NULL;
    args->stack_high = stack_high;
    args->stack_low = stack_low;

    return stack_high - sizeof(crt0_main_t) - PTR_SIZE;
}