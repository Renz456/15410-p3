/**
 * @file task.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _TASK_H
#define _TASK_H

#include <synchronization/mutex_kern.h>
#include <synchronization/list_helper.h>
#include <synchronization/cvar_kern.h>

typedef struct pcb
{
    int pid;
    int exited;

    void *stack_high;
    void *stack_low;

    void *page_directory;
    int num_threads;

    struct pcb *parent;
    struct pcb *child_list;
    struct pcb *zombie_list;

    mutex_t pcb_mp;
    cond_t pcb_cv;

    struct pcb *next;
    struct pcb *prev;
} pcb_t;

typedef struct entry_args
{
    int argc;
    char **argv;
    void *stack_high;
    void *stack_low;
} crt0_main_t;

pcb_t *create_pcb(void *page_directory);
void *init_task(pcb_t *pcb, char **argv, int argc);
void add_child(pcb_t **parent, pcb_t *child);
void remove_child(pcb_t **parent, pcb_t *child);

#endif /* _TASK_H */