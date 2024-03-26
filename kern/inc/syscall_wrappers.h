/**
 * @file gettid_wrapper.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _SYSCALL_WRAP_H
#define _SYSCALL_WRAP_H

/**
 * @brief Get the tid wrapper object
 *
 */
void gettid_wrapper(void);
void fork_wrapper(void);
void exec_wrapper(void);
void halt_wrapper(void);
void new_pages_wrapper(void);
void print_wrapper(void);
void readline_wrapper(void);
void wait_wrapper(void);
void task_vanish_wrapper(void);
void set_status_wrapper(void);

#endif /* _SYSCALL_WRAP_H */