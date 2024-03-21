/** @file deschedule.S
 *  @brief Stub routine for the deschedule system call
 *  
 *  Calls the deschedule system call by calling an interupt to EXEC_INT
 *  Input is loaded into %esi after its caller value is saved. 
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include <syscall_int.h>

.global exec

exec:
    pushl    %ebp            /* Save caller ebp */
    movl     %esp, %ebp      /* New EBP */
    pushl    %esi            /* Caller saved register */
    lea     8(%ebp), %esi    /* Move input to esi */
    int     $EXEC_INT       /* Call interupt */
    popl     %esi            /* Revert to saved esi */
    movl     %ebp, %esp      /* Restore caller stack pointer */
    popl     %ebp            /* Restore ebp */
    ret