/**
 * @file task_sys.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _TASK_SYS_H
#define _TASK_SYS_H

/**
 * @brief Struct for general purpose registers
 *        (I did not want the entirety of ureg)
 *
 */
typedef struct gen_reg
{
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int zero; /* Dummy %esp, set to zero */
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
} gen_reg_t;

int kernel_fork(gen_reg_t *regs);
int kernel_wait(int *status_ptr);
void kernel_set_status(int status);

#endif /* _TASK_SYS_H */