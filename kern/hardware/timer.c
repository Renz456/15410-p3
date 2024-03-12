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
    num_ticks++;
    tickback(num_ticks);
    // MAGIC_BREAK;
    if (num_ticks % 500 == 0)
        lprintf("A second has passed!\n");
    outb(INT_CTL_PORT, INT_ACK_CURRENT);
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