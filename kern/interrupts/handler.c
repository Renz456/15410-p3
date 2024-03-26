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
#include <assert.h>
#include <inc/vm.h>
#include <stdlib.h>
#include <cr.h>
#include <inc/asm_helpers.h>

void page_handler(int fault){
    assert(fault == 14);
    MAGIC_BREAK;
    unsigned int fault_VA = get_cr2();
    unsigned int fault_PA = (unsigned int)get_physical(fault_VA, (pde *)get_cr3());
    lprintf("fault VA %p, PA %p", (void *)fault_VA, (void *)fault_PA);
    if(fault_PA == ZFOD_ADDR_PA){
        // Check if the page needs to have some write access as such or naw
        void *frame_addr = get_frame_addr();
        if (frame_addr == NULL)
        {
            lprintf("No more pages left to assign");
            return;
        }
        if (add_frame(fault_VA, (unsigned int)frame_addr,
                      (pde *)((void *)get_cr3()), USER_PD_FLAG, USER_PT_FLAG) < 0)
        {
            lprintf("page already mapped, shouldnt reach here\n");
        }
        flush_page_entry(fault_VA);
        // Should I just assume if we try to write to this page then we j 
        // allocate page anyways or is there some case where the page 
        // initially anyways was supposed ot be read only
    }
    else{
        lprintf("actual page fault has occured %p", (void *)fault_VA);
        MAGIC_BREAK;
    }
}

void fault_handler(int fault)
{
    lprintf("Fault of type %d occurred !!! :(\n", fault);
    MAGIC_BREAK;
}