/**
 * @file halt_wrapper.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief 
 * 
 * @bug 
 * 
 */

#include <seg.h>

.global halt_wrapper

halt_wrapper:
    cli           /* clear interupts do i need this? */
    call sim_halt /* stop simics */
    hlt           /* halt hardware */
    iret 
