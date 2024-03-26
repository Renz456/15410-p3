/**
 * @file scheduler.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <inc/thread.h>

void initialise_scheduler();
void enable_contexts();
void disable_contexts();
void context_tickback(unsigned int tick);
void context_switch(int tid);
void add_to_run_queue(tcb_t *tcb, int is_new_thread);

void add_to_dead_queue(tcb_t *tcb);
tcb_t *remove_from_dead_queue();
void try_reap_tcb();

int kernel_yield(int tid);
int kernel_deschedule(int tid, int *reject);
int kernel_make_runnable(int tid);

#endif /* _SCHEDULER_H */