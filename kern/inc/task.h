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

typedef struct pcb
{
    int pid;
    // stack info
    void *stack_high;
    void *stack_low;
    // address space info
    void *page_directory;
    int num_threads;

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

#endif /* _TASK_H */