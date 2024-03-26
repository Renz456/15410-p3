/** @file set_status.S
 *  @brief Stub routine for the set_status system call
 *  
 *  Calls the set_status system call by calling an interupt to SET_STATUS_INT
 *  Input is loaded into %esi after its caller value is saved. 
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include <syscall_int.h>

.global set_status

set_status:
    pushl    %ebp               /* Save caller ebp */
    movl     %esp, %ebp         /* New EBP */
    pushl    %esi               /* Caller saved register */
    leal     8(%ebp), %esi      /* Move input to esi */
    int     $SET_STATUS_INT     /* Call interupt */
    popl     %esi               /* Revert to saved esi */
    movl     %ebp, %esp         /* Restore caller stack pointer */
    popl     %ebp               /* Restore ebp */
    ret