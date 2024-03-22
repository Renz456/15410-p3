/**
 * @file fork_wrapper.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <seg.h>
; #include <inc/task_sys.h>
; #include <inc/fork_wrapper.h>


/* should this become a regular syscall wrapper?? */

.global fork_wrapper

fork_wrapper:
    pushl %ebp
    mov   %esp, %ebp     
    sub   $4, %esp       /* Make space for return value */

    pusha

    mov %esp, %ecx      /* save this as the argument for kernel_fork */

    pushl %ds           /* Code segments are saved automatically*/
    pushl %es           /* Data segment and general purpose registers need to be manually saved*/
    pushl %fs 
    pushl %gs

    movl $SEGSEL_KERNEL_DS, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs

    pushl %ecx         /* push pointer to gen regs as argument */    
    call kernel_fork
    popl  %ecx              /* remove arg */
    /*movl  %eax,-4(%ebp) */  /* store result in the space that was saved earlier */
    movl  %eax,48(%esp)  /* removed ebp as that would be ebp in parent task stack */
    
    popl %gs
    popl %fs 
    popl %es 
    popl %ds
    popa

    popl %eax
    popl %ebp
    iret 
