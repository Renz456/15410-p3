/**
 * @file interupt.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _INTERUPT_KERN_H
#define _INTERUPT_KERN_H

void install_syscalls();
void install_fault_handlers();
void install_hardware_interrupts();

#endif /* _INTERUPT_KERN_H */