/**
 * @file timer.c
 * @brief File containing the implementation of the timer interupt handler
 * @author Rene Ravanan (rravanan)
 * @bug No known bugs
 */

#include <stdio.h>
#include <timer_defines.h>
#include <interrupt_defines.h>
#include <asm.h>
#include <simics.h>
#include <inc/thread.h>
#include <inc/scheduler.h>

void (*tickback)(unsigned int); // Global pointer to the tickback function
unsigned int num_ticks = 0;     // Counter for timer interupts

/**
 * @brief Interupt handler for timer. Upon interupt, an ack is sent to
 *        the control port, tickback is called and num ticks is incremented.
 *
 */
void timer_interupt_handler()
{
    // Call clock tick callback function
    // if (kernel_gettid() == 1)
    // {
    //     // MAGIC_BREAK;
    // }
    num_ticks++;

    // MAGIC_BREAK;

    outb(INT_CTL_PORT, INT_ACK_CURRENT); // THERER IS A BUG HERE!!
    if (num_ticks % 100 == 0)
    {
        try_reap_tcb();
        lprintf("A second has passed in thread %d!\n", get_tcb()->tid);
        // tickback(num_ticks);
        context_switch(-1);
        lprintf("ctx return in thread %d!\n", get_tcb()->tid);
        // enable_interrupts();
    }
}

/**
 * @brief Helper function for handler_install() to set the tickback function
 *
 * @param callback_fn : pointer to the given tickback function.
 */
void set_tickback(void (*callback_fn)(unsigned int))
{
    tickback = callback_fn;
}

int kernel_get_ticks()
{
    return num_ticks;
}
/*
00000000 <main>:
   0:   55                      push   %ebp
   1:   89 e5                   mov    %esp,%ebp
   3:   eb fe                   jmp    3 <main+0x3>
*/