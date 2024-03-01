/**
 * @file interupt_internal.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _INTERUPT_INTERNAL_H
#define _INTERUPT_INTERNAL_H

#include <stdint.h>

typedef struct
{
    short lsb_offset;
    short segment_select;
    uint8_t zero;
    uint8_t p_dpl_d;
    short msb_offset;
} table_entry_t;

#endif /* _INTERUPT_INTERNAL_H */