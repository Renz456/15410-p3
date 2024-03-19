/**
 * @file vm.h
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */

#ifndef _VM_H
#define _VM_H

typedef unsigned int pte;
typedef unsigned int pde;

#define TEN_BIT_MASK 0x3FF
#define MSB_SET_MASK 0x80000000

#define KERN_PD_FLAG 0b000100000011
#define KERN_PT_FLAG 0b000100000011

#define USER_PD_FLAG 0b000000000111
#define USER_PT_FLAG 0b000000000111
#define CLEAR_BOTTOM 0xFFFFF000
#define PRESENT_BIT_MASK 0x1
#define SUPERVISOR_BIT_MASK 0x4

#define COPY_ADDR_VA 0x0f000000

int add_frame(unsigned int virtual_address, unsigned int physical_address, pde *pd_start, int pd_flags, int pt_flags);
void map_kernel_space(pde *pd_start);
void initialize_vm();
int get_pt_index(void *entry);
int get_pd_index(void *entry);
void *get_frame_addr();
int new_pages(void *addr, int len);
int align_pages(void *addr, int size);
void *clone_page_directory(void *old_pd);

#endif /* _VM_H */