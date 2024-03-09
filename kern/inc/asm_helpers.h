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

#endif /* _ASM_HELPER_KERN_H */