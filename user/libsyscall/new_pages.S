/** @file new_pages.S
 *  @brief Stub routine for the new_pages system call
 *  
 *  Calls the new_pages system call by calling an interupt to NEW_PAGES_INT
 *  Input is loaded into %esi after its caller value is saved. 
 *  
 *  @author Rene Ravanan (rravanan)
 *  @author Abhinav Gupta (abhinav6)
 */

#include<syscall_int.h>

.global new_pages

new_pages:
    pushl    %ebp           /* Save caller ebp */
    movl     %esp, %ebp     /* New EBP */
    pushl    %esi           /* Caller saved register */
    lea     8(%ebp), %esi   /* Move input to esi */
    int     $NEW_PAGES_INT  /* Call interupt */
    popl     %esi           /* Revert to saved esi */
    movl     %ebp, %esp     /* Restore caller stack pointer */
    popl     %ebp           /* Restore ebp */
    ret 
