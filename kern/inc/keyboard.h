/**
 * @file keyboard.h
 * @brief Header file for keyboard interupts,
 *        readchar() and readline() are documented in p1kern.h
 * @author Rene Ravanan (rravanan)
 *
 * @bug No known bugs
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/**
 * @brief Interupt handler for keyboard. Confirms acknledgement to control
 *        port and adds the inputed characters to a buffer to be processed
 *        later.
 *
 */
void keyboard_interupt_handler();

int readchar(void);
int readline(int len, char *buf);
void install_keyboard();

#endif /* _KEYBOARD_H */