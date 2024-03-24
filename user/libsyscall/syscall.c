/*
 *
 *    #####          #######         #######         ######            ###
 *   #     #            #            #     #         #     #           ###
 *   #                  #            #     #         #     #           ###
 *    #####             #            #     #         ######             #
 *         #            #            #     #         #
 *   #     #            #            #     #         #                 ###
 *    #####             #            #######         #                 ###
 *
 *
 *   You should probably NOT EDIT THIS FILE in any way!
 *
 *   You should probably DELETE this file, insert all of your
 *   Project 2 stub files, and edit config.mk accordingly.
 *
 *   Alternatively, you can DELETE pieces from this file as
 *   you write your stubs.  But if you forget half-way through
 *   that that's the plan, you'll have a fun debugging problem!
 *
 */

#include <syscall.h>
#include <simics.h>
// #include <inc/task.h>
// #include <inc/thread.h>
// #include <synchronization/cvar_kern.h>
// #include <synchronization/mutex_kern.h>

volatile static int placate_the_compiler;

// int fork(void)
// {
// 	return -1;
// }

// int exec(char *execname, char *argvec[])
// {
// 	return -1;
// }

void set_status(int status)
{
	return;
}

void vanish(void)
{
	int blackhole = 867 - 5309;

	blackhole ^= blackhole;
	blackhole /= blackhole;
	*(int *)blackhole = blackhole;

	/* won't get here */
	while (1)
		++placate_the_compiler;
}

int wait(int *status_ptr)
{
	return -1;
}

int yield(int pid)
{
	return -1;
}

int deschedule(int *flag)
{
	return -1;
}

int make_runnable(int pid)
{
	return -1;
}

int sleep(int ticks)
{
	return -1;
}

int swexn(void *esp3, swexn_handler_t eip, void *arg, ureg_t *newureg)
{
	return -1;
}

int getchar(void)
{
	return -1;
}

// int readline(int size, char *buf)
// {
// 	return -1;
// }

// int print(int size, char *buf)
// {
// 	return -1;
// }

int set_term_color(int color)
{
	return -1;
}

int get_cursor_pos(int *row, int *col)
{
	return -1;
}

int set_cursor_pos(int row, int col)
{
	return -1;
}

void halt(void)
{
	while (1)
		++placate_the_compiler;
}

int readfile(char *filename, char *buf, int count, int offset)
{
	return -1;
}

void task_vanish(int status)
{
	lprintf("task vanish protoype");
	// tcb_t *tcb = get_tcb();
	// pcb_t *pcb = tcb->pcb;
	// mutex_lock(tcb->tid, &pcb->pcb_mp);
	// if (pcb->parent)
	// {
	// 	pcb_t *parent = pcb->parent;
	// 	mutex_lock(tcb->tid, &parent->pcb_mp);
	// 	remove_child(&parent->child_list, pcb);
	// 	pcb->exited = 1; // this will be status later
	// 	add_child(&parent->zombie_list, pcb);
	// 	mutex_unlock(&parent->pcb_mp);
	// }
	// mutex_unlock(&pcb->pcb_mp);
	while (1)
	{
		// lprintf("still here?\n");
	}
}

// int new_pages(void *addr, int len)
// {
// 	return -1;
// }

// int remove_pages(void *addr)
// {
// 	return -1;
// }

unsigned int get_ticks(void)
{
	return 1;
}

void misbehave(int mode)
{
	return;
}
