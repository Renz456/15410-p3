/**
 * @file task_sys.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug NEED TO IMPLEMENT
 *
 */

#include <x86/cr.h>
#include <inc/task.h>
#include <inc/thread.h>
#include <inc/scheduler.h>
#include <inc/task_sys.h>
#include <inc/kern_constants.h>
#include <loader.h>
#include <elf_410.h>
#include <inc/vm.h>
#include <simics.h>
#include <stdio.h>
#include <string.h>
#include <inc/asm_helpers.h>

/**
 * @brief Set the child task's stack. From the assembly code, the return address
 *        of fork_wrapper will be stored 24 bytes down from where pusha (general
 *        purpose regs) are stored. So selecting this address will give the return
 *        address to set in the child process. We also use this as a reference from
 *        which to copy the parent stack. Gen regs are the pushed again so they can
 *        be used in the popa after a context switch.
 *
 * @param tcb
 * @param regs
 * @param cur_tcb
 */
void set_child_stack(tcb_t *tcb, gen_reg_t *regs, tcb_t *cur_tcb)
{
  /* esp i want : 0x00148fa4*/
  /* eip i want: bc21 1000 = 001021bc */
  // TODO explain this magic number
  void *old_esp = (void *)((uint32_t)regs - 6 * PTR_SIZE);
  unsigned int stack_offset = (unsigned int)cur_tcb->kernel_stack - ((unsigned int)old_esp);
  tcb->esp = tcb->kernel_stack - stack_offset;
  memcpy(tcb->esp, (void *)old_esp, stack_offset);
  gen_reg_t *new_regs = tcb->esp - sizeof(gen_reg_t);
  new_regs->edi = regs->edi;
  new_regs->esi = regs->esi;
  new_regs->ebp = regs->ebp;
  new_regs->zero = regs->zero;
  new_regs->ebx = regs->ebx;
  new_regs->edx = regs->edx;
  new_regs->ecx = regs->ecx;
  new_regs->eax = 0; // child should return 0 from fork
  tcb->esp = (void *)new_regs;
}

int kernel_fork(gen_reg_t *regs)
{
  /*
  Assign a new task id

  Deep copy of parent address space
  Do I need to copy all the threads its running too?
  should we not allow process to fork if it has more than one thread??
  Create kernel

  */

  // 3c1f 1500
  // MAGIC_BREAK;
  void *old_esp = get_esp();
  lprintf("what esp is this %p\n", old_esp);
  void *cur_pd = (void *)get_cr3();

  void *new_pd = clone_page_directory(cur_pd);
  lprintf("Parent successful clone\n");
  pcb_t *new_pcb = create_pcb(new_pd);
  // void *stack = init_task(new_pcb); // I should not need to do this
  tcb_t *new_tcb = create_tcb(new_pcb); // I need this since there's a new kernel stack

  /*
  Set the new threads registers etc. on the stack
  - somehow get current threads pusha, copy that onto the
    new threads kernel stack, set eax to 0?

  Add this to a runnnable queue
  */
  set_child_stack(new_tcb, regs, get_tcb());
  add_to_run_queue(new_tcb, 0);

  lprintf("Parent exiting syscall\n");
  return new_pcb->pid;
}

int get_arg_len(char *arg_arr[])
{
  int i = 0;
  while (arg_arr[i])
  {
    i++;
  }
  return i;
}

int kernel_exec(char *execname, char *argvec[])
{
  /* should not execute if there are more than one threads in process*/
  /* for the asm wrapper, esi will store the filename and argv pointers */
  /* should maybe come up with a generic asm wrapper? */
  /* TODO: should make an error code enum for all int functions */

  tcb_t *tcb = get_tcb();
  pcb_t *pcb = tcb->pcb;

  if (pcb->num_threads > 1)
  {
    return -1;
  }

  // if (validate_string_array(argvec) < 0)
  // {
  //   return -2;
  // }

  // int exec_len = validate_string(execname);

  int argc = get_arg_len(argvec);

  void *user_stack = init_task(pcb, (char **)argvec, argc);

  simple_elf_t se_hdr;
  if (elf_load_helper(&se_hdr, execname) < 0)
  {
    lprintf("ELF exec file does not exist\n");
    return -3;
  }

  if (init_address_space(execname) < 0)
  {
    return -4;
  }

  void *eip = (void *)se_hdr.e_entry;

  usr_state_t user_state = set_user_state(tcb, user_stack, eip);
  exec_user(user_state);

  return -5;
}

/*
Not really sure where else I would need this so Im writing notes about locking here

We definitely need a lock for the run/sleep/block queues

Since no spin waiting is allowed, mutex/cvar implementations could be just descheduling until lock is acquired?

Should maybe also lock some vm stuffs? Like the free frame list,

*/