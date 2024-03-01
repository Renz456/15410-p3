/**
 * @file asm_helpers.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <thread.h>

void *get_esp();

void exec_user(usr_state_t *user_state);