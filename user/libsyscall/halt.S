/** @file halt.S
 *  @brief Stub routine for the halt system call
 *  
 *  Calls the halt system call by calling an interupt to HALT_INT
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */


#include<syscall_int.h>

.global halt

halt:
    pushl    %ebp        /* Save caller ebp */
    movl     %esp, %ebp  /* New EBP */
    int     $HALT_INT    /* Call interupt */
    movl     %ebp, %esp  /* Restore caller stack pointer */
    popl     %ebp        /* Restore ebp */
    ret 