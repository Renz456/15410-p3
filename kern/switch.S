/**
 * @file switch.S
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */



 .global finish_switch

 finish_switch:
    pusha                 /* save general purpose reg of current thread */
    movl   36(%esp), %eax /* load first argument to eax (old esp) */
    movl   %esp, (%eax)   /* save esp in node */
    movl   40(%esp), %eax /* load second arg (new esp) */
    leal   %eax, %esp     /* set new thread esp */
    popa                  /* restore general registers of new thread */
