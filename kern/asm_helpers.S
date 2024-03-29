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

    leal 8(%ebp), %eax
    /* pushl %eax */                  /* push user state onto stack */
    leal (%eax),%esp
    /*movl $0xffffefec,%eax
    movl $0xffffefec,%ebx
    movl $0xffffefec,%ecx
    movl $0xffffefec,%edx
    movl $0xffffefec,%esi
    movl $0xffffefec,%edi */
    iret

.global xchg_wrapper

xchg_wrapper:
    mov 8(%esp), %eax //Loading 2nd arg
    mov 4(%esp), %ecx //Loading 1st arg
    xchg (%ecx), %eax //Calling the xchg function
    ret //Returning from the xchg function


/* Wrapper to call the timer interupt handler 
   after saving callee registers*/
.global  timer_wrapper
timer_wrapper:
    pusha
    call timer_interupt_handler
    popa
    IRET

/* Wrapper to call the keyboard interupt handler 
   after saving callee registers*/

.global  keyboard_wrapper
keyboard_wrapper:
    pusha
    /* call keyboard_interupt_handler */
    popa
    IRET


