/**
 * @file handler.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <simics.h>

void fault_handler(int fault)
{
    lprintf("Fault of type %d occurred !!! :(", fault);
    MAGIC_BREAK;
}