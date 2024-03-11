/**
 * @file timer.h
 * @brief Header file for timer interupt interface
 * @author Rene Ravanan (rravanan)
 *
 * @bug No known bugs
 */

#ifndef _TIMER_H
#define _TIMER_H

/**
 * @brief Interupt handler for timer, as calld by assembly wrapper.
 *        Confirms acknowledgement to Control port, updates number of
 *        ticks and calls tickback function
 *
 */
void timer_interupt_handler(void);

/**
 * @brief Set the tickback object given to handler_install()
 *
 * @param callback_fn : pointer to tickback function for timer interupt
 */
void set_tickback(void (*callback_fn)(unsigned int));

#endif /* _TIMER_H */