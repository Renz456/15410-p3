/** @file get_ticks.S
 *  @brief Stub routine for the get_ticks system call
 *  
 *  Calls the get_ticks system call by calling an interupt to GET_TICKS_INT
 *  Input is loaded into %esi after its caller value is saved. 
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include <syscall_int.h>

.global get_ticks

get_ticks:
    pushl    %ebp            /* Save caller ebp */
    movl     %esp, %ebp      /* New EBP */
    pushl    %esi            /* Caller saved register */
    lea     8(%ebp), %esi   /* Move input to esi */
    /* int     $GET_TICKS_INT */   /* Call interupt */
    mov $1, %eax
    popl     %esi            /* Revert to saved esi */
    movl     %ebp, %esp      /* Restore caller stack pointer */
    popl     %ebp            /* Restore ebp */
    ret

