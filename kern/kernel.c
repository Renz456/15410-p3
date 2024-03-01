/** @file kernel.c
 *  @brief An initial kernel.c
 *
 *  You should initialize things in kernel_main(),
 *  and then run stuff.
 *
 *  @author Rene Ravanan (rravanan)
 *          Abhinav Gupta (abhinav6)
 *  @bug No known bugs.
 */

#include <common_kern.h>

/* libc includes. */
#include <stdio.h>
#include <simics.h> /* lprintf() */

/* multiboot header file */
#include <multiboot.h> /* boot_info */

/* x86 specific includes */
#include <x86/asm.h> /* enable_interrupts() */

#include <interupt.h>
#include <thread.h>
#include <task.h>
#include <inc/loader.h>
#include <exec2obj.h>
#include <elf_410.h>

#define STARTING_FILE "idle"

volatile static int __kernel_all_done = 0;

/** @brief Kernel entrypoint.
 *
 *  This is the entrypoint for the kernel.
 *
 * @return Does not return
 */
int kernel_main(mbinfo_t *mbinfo, int argc, char **argv, char **envp)
{
    // placate compiler
    (void)mbinfo;
    (void)argc;
    (void)argv;
    (void)envp;

    install_fault_handlers();
    install_syscalls();
    /*
     * When kernel_main() begins, interrupts are DISABLED.
     * You should delete this comment, and enable them --
     * when you are ready.
     */

    // init pcb?
    void *page_directory;
    pcb_t *pcb = create_pcb(page_directory);
    void *stack = init_task(pcb);
    // void *esp;
    tcb_t *tcb = create_tcb(pcb);

    lprintf("Hello from a brand new kernel!\n");

    int app_index = find_app_index(STARTING_FILE);
    if (app_index < 0)
    {
        lprintf("Invalid starting app!\n");
        return -1;
    }

    if (init_address_space(STARTING_FILE) < 0)
    {
        lprintf("Starting app address space failed!\n");
        return -1;
    }

    simple_elf_t se_hdr;
    if (elf_load_helper(&se_hdr, STARTING_FILE) < 0)
    {
        lprintf("this should not happen\n");
        return -1;
    }

    void *eip = (void *)se_hdr.e_entry;

    run_thread(tcb, stack, eip);
    while (!__kernel_all_done)
    {
        continue;
    }

    return 0;
}
