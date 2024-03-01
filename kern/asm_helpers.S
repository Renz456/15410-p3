/**
 * @file asm_helpers.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

 .global get_esp

/**
 * @brief Implements function in assmebly to retrun function frame pointer
 *
 * @param void
 * @return Address of stack frame pointer 
 */
get_esp:
    movl %eso,%eax
    ret

.global exec_user

exec_user:
    pushl %ebp
    movl %esp, %ebp /* save stack frame and set new one */

    mov $SEGSEL_USER_DS, %eax /* load data segment */
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs

    mov $8(%ebp), %eax
    pushl %eax                  /* push user state onto stack */
    iret






