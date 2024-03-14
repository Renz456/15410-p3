/**
 * @file gettid_wrapper.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <seg.h>
; #include <inc/gettid_wrapper.h>


/* should this become a regular syscall wrapper?? */

.global gettid_wrapper

gettid_wrapper:
    pushl %ebp
    mov   %esp, %ebp     
    sub   $4, %esp       /* Make space for return value */

    pusha
    pushl %ds           /* Code segments are saved automatically*/
    pushl %es           /* Data segment and general purpose registers need to be manually saved*/
    pushl %fs 
    pushl %gs

    movl $SEGSEL_KERNEL_DS, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs

    call kernel_gettid

    mov  %eax,-4(%ebp)  

    popl %gs
    popl %fs 
    popl %es 
    popl %ds
    popa

    popl %eax
    popl %ebp
    iret 
