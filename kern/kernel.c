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
#include <inc/vm.h>
#include <x86/cr.h>
#include <inc/scheduler.h>
#include <inc/kern_constants.h>

#define STARTING_FILE "fork_test1"
#define IDLE_FILE "idle"

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
    lprintf("Installed fault handlers!\n");
    install_syscalls();
    lprintf("Installed Sycalls!\n");
    install_hardware_interrupts();
    lprintf("Installed Hardware Interupts!\n");
    initialize_vm();
    lprintf("Installed VM!\n");
    initialise_scheduler();

    /*
     * When kernel_main() begins, interrupts are DISABLED.
     * You should delete this comment, and enable them --
     * when you are ready.
     */

    // init pcb?
    void *page_directory = (void *)get_cr3();
    // void *idle_pd = clone_page_directory(page_directory);
    pcb_t *pcb = create_pcb(page_directory);
    void *stack = init_task(pcb, NULL, 0);
    // void *esp;
    lprintf("process actual start stack %p\n", stack);
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

    lprintf("Entry addr %lx\n", se_hdr.e_entry);

    // pde *idle_pd = malloc(sizeof(pde));

    // set_cr3((unsigned int)idle_pd);
    // pcb_t *idle_pcb = create_pcb(idle_pd);
    // void *idle_stack = init_task(idle_pcb, NULL, 0);
    // tcb_t *idle_tcb = create_tcb(idle_pcb);
    // init_address_space(IDLE_FILE);
    // simple_elf_t se_hdr_idle;
    // if (elf_load_helper(&se_hdr_idle, STARTING_FILE) < 0)
    // {
    //     lprintf("this should not happen\n");
    //     return -1;
    // }
    // prepare_thread(idle_tcb, idle_stack, (void *)se_hdr_idle.e_entry);
    // set_cr3((uint32_t)page_directory);

    run_thread(tcb, stack, eip);

    lprintf("starting while loop\n");
    while (!__kernel_all_done)
    {
        continue;
    }

    return 0;
}
