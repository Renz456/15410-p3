#include <stdint.h>

typedef struct
{
    short lsb_offset;
    short segment_select;
    uint8_t zero;
    uint8_t p_dpl_d;
    short msb_offset;
} table_entry_t;