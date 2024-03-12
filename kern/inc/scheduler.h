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

void initialise_scheduler();
void enable_contexts();
void disable_contexts();
void context_tickback(unsigned int tick);
void context_switch(int tid);

#endif /* _SCHEDULER_H */