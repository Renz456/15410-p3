/**
 * @file get_ticks_wrapper.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <seg.h>
; #include <inc/task_sys.h>
; #include <inc/get_ticks_wrapper.h>


/* should this become a regular syscall wrapper?? */

.global get_ticks_wrapper

get_ticks_wrapper:
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

    call kernel_get_ticks
    movl  %eax,-4(%ebp)  /* store result in the space that was saved earlier */
    
    popl %gs
    popl %fs 
    popl %es 
    popl %ds
    popa

    popl %eax
    popl %ebp
    iret 
