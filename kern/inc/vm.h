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

int add_frame(unsigned int virtual_address, unsigned int physical_address, void *pd_start);
void map_kernel_space(void *pd_start);
void initialize_vm();
int get_pt_index(void *entry);
int get_pd_index(void *entry);
void *get_frame_addr();
int new_pages(void *addr, int len);
int align_pages(void *addr, int size);
#endif /* _VM_H */