/**
 * @file interupt.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#include <stdint.h>
#include <fault_asm_wrappers.h>
#include <interupt_internal.h>
#include <seg.h>

#define TRAP_GATE_CONFIG 0x0F // 0D111 where D is set to 1 for 32 bit gates
#define SEGMENT_PRESENT 1 << 7
#define KERNEL_DPL 0 << 5
#define OFFSET_SIZE 16 // Offset size (in bits) in intel trap gate

/**
 * @brief Create a idt entry object
 *
 * @param handler
 * @param dpl
 * @param table_index
 */
void create_idt_entry(void *handler, uint8_t dpl, int table_index)
{
    table_entry_t *idt = (table_entry_t *)idt_base();
    table_entry_t table_entry;
    table_entry.lsb_offset = (short)(uintptr_t)handler;
    table_entry.segment_select = SEGSEL_KERNEL_CS; // as per handout
    table_entry.zero = 0;
    table_entry.p_dpl_d = SEGMENT_PRESENT | KERNEL_DPL | TRAP_GATE_CONFIG;
    table_entry.msb_offset = (short)((uintptr_t)handler >> OFFSET_SIZE);
    idt[table_index] = table_entry;
}

/**
 * @brief
 *
 */
void install_fault_handlers()
{
    create_idt_entry(de_wrapper, KERNEL_DPL, 0);
    create_idt_entry(db_wrapper, KERNEL_DPL, 1);
    create_idt_entry(nmi_wrapper, KERNEL_DPL, 2);
    create_idt_entry(bp_wrapper, KERNEL_DPL, 3);
    create_idt_entry(of_wrapper, KERNEL_DPL, 4);
    create_idt_entry(br_wrapper, KERNEL_DPL, 5);
    create_idt_entry(ud_wrapper, KERNEL_DPL, 6);
    create_idt_entry(nm_wrapper, KERNEL_DPL, 7);
    create_idt_entry(df_wrapper, KERNEL_DPL, 8);
    create_idt_entry(cop_wrapper, KERNEL_DPL, 9);
    create_idt_entry(ts_wrapper, KERNEL_DPL, 10);
    create_idt_entry(np_wrapper, KERNEL_DPL, 11);
    create_idt_entry(ss_wrapper, KERNEL_DPL, 12);
    create_idt_entry(gp_wrapper, KERNEL_DPL, 13);
    create_idt_entry(pf_wrapper, KERNEL_DPL, 14);
    create_idt_entry(mf_wrapper, KERNEL_DPL, 16);
    create_idt_entry(ac_wrapper, KERNEL_DPL, 17);
    create_idt_entry(mc_wrapper, KERNEL_DPL, 18);
}
