/** @file gettid.S
 *  @brief Stub routine for the gettid system call
 *  
 *  Calls the gettid system call by calling an interupt to GETTID_INT
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include<syscall_int.h>

.global gettid

gettid:
    pushl    %ebp           /* Save caller ebp */
    movl     %esp, %ebp     /* New EBP */
    int     $GETTID_INT     /* Call interupt */
    movl     %ebp, %esp     /* Restore caller stack pointer */
    popl     %ebp           /* Restore ebp */
    ret 
