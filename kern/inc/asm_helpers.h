/**
 * @file asm_helpers.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _ASM_HELPER_KERN_H
#define _ASM_HELPER_KERN_H

#include <thread.h>

void *get_esp();

void exec_user(usr_state_t user_state);
int xchg_wrapper(int *, int);
void timer_wrapper();
void keyboard_wrapper();
void new_switch(void **old_esp, void *new_esp);
void finish_switch(void **old_esp, void *new_esp);
int atomic_increment(int *n);
void flush_page_entry(unsigned int address);

#endif /* _ASM_HELPER_KERN_H */
