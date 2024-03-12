/** @file fork.S
 *  @brief Stub routine for the fork system call
 *  
 *  Calls the fork system call by calling an interupt to FORK_INT
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include<syscall_int.h>

.global fork

fork:
    pushl    %ebp        /* Save caller ebp */
    movl     %esp, %ebp  /* New EBP */
    int     $FORK_INT   /* Call interupt */
    movl     %ebp, %esp  /* Restore caller stack pointer */
    popl     %ebp        /* Restore ebp */
    ret 