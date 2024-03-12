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
#include <inc/asm_helpers.h>
#include <inc/timer.h>
#include <timer_defines.h>
#include <inc/keyboard.h>
#include <keyhelp.h>
#include <gettid_wrapper.h>
#include <interupt_internal.h>
#include <syscall_int.h>
#include <interupt.h>
#include <seg.h>
#include <x86/asm.h>
#include <vm.h>
#include <simics.h>

#define TRAP_GATE_CONFIG 0x0F // 0D111 where D is set to 1 for 32 bit gates
#define SEGMENT_PRESENT 1 << 7
#define KERNEL_DPL 0 << 5
#define USER_DPL 3 << 5
#define OFFSET_SIZE 16 // Offset size (in bits) in intel trap gate
// We want interupt every 10ms and pc cycle is 1193182Hz
#define TIMER_INT_FREQ 11931

static void create_idt_entry(void *handler, uint8_t dpl, int table_index);

void install_syscalls()
{
    create_idt_entry(gettid_wrapper, USER_DPL, GETTID_INT);
    create_idt_entry(new_pages, USER_DPL, NEW_PAGES_INT);
}

/**
 * @brief TODO: this needs to do something
 *                  - Try context switch
 *                  - Move stuff from the sleep queue
 *
 *
 * @param tick
 */
void timer_tickback(unsigned int tick)
{
    return;
}

/**
 * @brief Initialises the timer and sets interrupt frequency
 *
 */
void install_timer()
{
    short int_cyc = TIMER_INT_FREQ;
    outb(TIMER_MODE_IO_PORT, TIMER_SQUARE_WAVE);
    outb(TIMER_PERIOD_IO_PORT, int_cyc);
    outb(TIMER_PERIOD_IO_PORT, int_cyc >> 8);
}

void install_hardware_interrupts()
{
    install_timer();
    set_tickback(timer_tickback);
    create_idt_entry(timer_wrapper, KERNEL_DPL, TIMER_IDT_ENTRY);
    lprintf("timer installed?\n");
    // create_idt_entry(keyboard_wrapper, KERNEL_DPL, KEY_IDT_ENTRY);
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

/**
 * @brief Create a idt entry object
 *
 * @param handler
 * @param dpl
 * @param table_index
 */
static void create_idt_entry(void *handler, uint8_t dpl, int table_index)
{
    table_entry_t *idt = (table_entry_t *)idt_base();
    table_entry_t table_entry;
    table_entry.lsb_offset = (short)(uintptr_t)handler;
    table_entry.segment_select = SEGSEL_KERNEL_CS; // as per handout
    table_entry.zero = 0;
    table_entry.p_dpl_d = SEGMENT_PRESENT | dpl | TRAP_GATE_CONFIG;
    table_entry.msb_offset = (short)((uintptr_t)handler >> OFFSET_SIZE);
    idt[table_index] = table_entry; // TODO: check the alignment in case smth goes wrong
}