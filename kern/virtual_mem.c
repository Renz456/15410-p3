/**
 * @file virtual_mem.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug
 *
 */
#include <malloc.h>
#include <page.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <common_kern.h>
#include <cr.h>

#define TEN_BIT_MASK 0xFFFFF
#define MSB_SET_MASK 0x80000000

#define PRESENT_BIT_MASK 0x1

void *frame_curr = 0x01000000;

int check_present(unsigned int entry)
{
    return (entry & PRESENT_BIT_MASK);
}

// Need to check for flags
int add_frame(unsigned int virtual_address, unsigned int physical_address, void *pd_start)
{
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    // Check if the pd_start[pd_idx] is present otherwise create
    // Check if pd_start[pd_idx][pt_idx] is present otherwise create
    // Once both and creating then store frame in pd_start[pd_idx][pt_idx]
    // Need to read about the bottom 12 bit flags associated with both entries
    if (!check_present(*(unsigned int *)(pd_start + pd_idx)))
    {
        void *new_page_table = smemalign(PAGE_SIZE, PAGE_SIZE);
        memset(new_page_table, 0, PAGE_SIZE);
        *(unsigned int *)(pd_start + pd_idx) = (unsigned int)new_page_table; // should set flags here
    }
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);
    void *page_table = *(unsigned int *)(pd_start + pd_idx);
    if (check_present(*(unsigned int *)(page_table + pt_idx)))
    {
        panic("page already seems to be mapped");
    }
    (*(unsigned int *)(page_table + pt_idx)) = (unsigned int)physical_address;
}

void map_kernel_space(void *pd_start)
{
    for (unsigned int map_start = 0; map_start < USER_MEM_START; map_start += PAGE_SIZE)
    {
        if (add_frame(map_start, map_start, pd_start) < 0)
        {
            panic("could not do kernel direct mapping");
        }
    }
}

void initialize_vm()
{
    void *pd_start = smemalign(PAGE_SIZE, PAGE_SIZE);
    map_kernel_space(pd_start);
    set_cr3((unsigned int)pd_start);
    set_cr0(get_cr0() | MSB_SET_MASK);
}

int get_pt_index(void *entry)
{
    return (((unsigned int)entry >> 12) & TEN_BIT_MASK);
}

int get_pd_index(void *entry)
{
    return (((unsigned int)entry >> 22) & TEN_BIT_MASK);
}

void *get_frame_addr()
{
    void *ret_frame = frame_curr;
    frame_curr += PAGE_SIZE;
    if (ret_frame > machine_phys_frames() * PAGE_SIZE)
    {
        return NULL;
    }
    return ret_frame;
}

int new_pages(void *addr, int len)
{
    assert(addr != NULL);
    if (len % PAGE_SIZE != 0)
    {
        panic("len is not page alinged");
    }

    unsigned int base_addr = (unsigned int)addr;

    if (base_addr % PAGE_SIZE != 0)
    {
        panic("base is not page aligned");
    }

    if (base_addr < USER_MEM_START)
    {
        panic("User space trying to access kernel memory");
    }

    for (unsigned int start = base_addr; start < base_addr + len; start += PAGE_SIZE)
    {
        void *frame_addr = get_frame_addr();
        if (frame_addr == NULL)
        {
            panic("No more pages left to assign");
        }
        add_frame(start, frame_addr, get_cr3());
    }
}

int align_pages(void *addr, int size)
{
    addr;
    size;
    return 0;
}