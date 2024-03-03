/**
 * @file asm_helpers.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <seg.h>

/**
 * @brief Implements function in assmebly to retrun function stack pointer
 *
 * @param void
 * @return Address of stack pointer 
 */
.global get_esp
get_esp:
    movl %esp,%eax
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

    movl 8(%ebp), %eax
    /* pushl %eax */                  /* push user state onto stack */
    leal (%eax),%esp
    /*movl $0xffffefec,%eax
    movl $0xffffefec,%ebx
    movl $0xffffefec,%ecx
    movl $0xffffefec,%edx
    movl $0xffffefec,%esi
    movl $0xffffefec,%edi */
    iret






