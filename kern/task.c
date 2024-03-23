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
#include <synchronization/mutex_kern.h>
#include <synchronization/cvar_kern.h>
// #include <vm.h>

#define TASK_PAGE_SIZE PAGE_SIZE
int next_pid = 0;

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
    assert(parent && child);
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
