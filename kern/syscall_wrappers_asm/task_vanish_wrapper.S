/**
 * @file task_vanish_wrapper.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <seg.h>
#include <simics.h>


/* should this become a regular syscall wrapper?? */

.global task_vanish_wrapper

task_vanish_wrapper:
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

    movl (%esi), %eax
    pushl %eax

    call kernel_task_vanish
    
    mov  %eax,-4(%ebp) 
    /* movl  %eax,52(%esp) */ /* store result on stack */
    
    popl %eax   /* Clear input args */

    popl %gs
    popl %fs 
    popl %es 
    popl %ds
    popa

    popl %eax
    popl %ebp
    iret 
