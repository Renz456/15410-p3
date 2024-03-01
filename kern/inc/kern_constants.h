/**
 * @file kern_constants.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _KERN_CONSTANTS_H
#define _KERN_CONSTANTS_H

#include <syscall.h>

#define PAGE_MASK (~(PAGE_SIZE - 1))
#define KERNEL_PAGE_SIZE PAGE_SIZE
#define KERNEL_PAGE_MASK (~(KERNEL_PAGE_SIZE - 1))
#define WORD_SIZE 4
#define USER_MEM_START 0xFFFFFFFF

#endif /* _KERN_CONSTANTS_H */