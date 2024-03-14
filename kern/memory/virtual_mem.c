/**
 * @file virtual_mem.c
 * @author Rene Ravanan (rravanan)
 *         Abhinav Gupta (abhinav6)
 * @brief
 *
 * @bug if new pages fails in the middle of allocating 
 * then the prev pages r still being allocated
 *  they need to be reused/freed
 *
 */
#include <malloc.h>
#include <page.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <common_kern.h>
#include <cr.h>
#include <inc/kern_constants.h>
#include <vm.h>
#include <simics.h>

void *frame_curr = (void *)USER_MEM_START;

int get_pt_index(void *entry)
{
    return (((unsigned int)entry >> 12) & TEN_BIT_MASK);
}

int get_pd_index(void *entry)
{
    return (((unsigned int)entry >> 22) & TEN_BIT_MASK);
}

int check_present(void *entry)
{
    return ((unsigned int)entry & PRESENT_BIT_MASK);
}

unsigned int set_flags(void *entry, int flags)
{
    return ((unsigned int)entry | flags);
}

pde *create_new_pd()
{
    pde *pd_new = smemalign(PAGE_SIZE, PAGE_SIZE); // Allocating array of 1024 unsigned ints
    // pde *pd_new = smalloc(PAGE_SIZE);
    assert(pd_new);
    memset(pd_new, 0, PAGE_SIZE); // Setting all to 0 in the start
    return pd_new;
}

// Need to check for flags
int add_frame(unsigned int virtual_address, unsigned int physical_address, pde *pd_start, int pd_flags, int pt_flags)
{
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    // Check if the pd_start[pd_idx] is present otherwise create
    // Check if pd_start[pd_idx][pt_idx] is present otherwise create
    // Once both and creating then store frame in pd_start[pd_idx][pt_idx]
    // Need to read about the bottom 12 bit flags associated with both entries
    if (!check_present((void *)pd_start[pd_idx])) // Checking if the directory is present or not, if not then we create it and set the PD flags
    {
        pde *new_page_table = create_new_pd();
        // lprintf("check addr %x new pt %p\n", new_page_table[get_pt_index((void *)virtual_address)], new_page_table);
        // new_page_table = (pde *)set_flags(new_page_table, pd_flags);
        pd_start[pd_idx] = set_flags(new_page_table, pd_flags);
    }
    pte *pt_start = (pte *)(pd_start[pd_idx] & CLEAR_BOTTOM);
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);

    // lprintf("check pte %x check pt index %d ptstart %p\n", pt_start[pt_idx], pt_idx, pt_start);

    // void *page_table = (void *)(*(unsigned int *)(pd_start + pd_idx));
    if (check_present((void *)pt_start[pt_idx]))
    {
        // lprintf("page already seems to be mapped %x\n", pt_start[pt_idx]);
        return -1;
    }
    pt_start[pt_idx] = (pte)(physical_address | pt_flags);
    return 0;
}

void map_kernel_space(pde *pd_start)
{
    for (unsigned int map_start = 0; map_start < USER_MEM_START; map_start += PAGE_SIZE)
    {
        if (add_frame(map_start, map_start, pd_start, KERN_PD_FLAG, KERN_PT_FLAG) < 0)
        {
            panic("could not do kernel direct mapping");
        }
    }
}

void initialize_vm()
{
    pde *pd_start = create_new_pd();
    map_kernel_space(pd_start);
    set_cr3((unsigned int)pd_start);
    set_cr0(get_cr0() | CR0_PG | CR0_PE);
}

void *get_frame_addr()
{
    void *ret_frame = frame_curr;
    frame_curr += PAGE_SIZE;
    if ((unsigned int)ret_frame > machine_phys_frames() * PAGE_SIZE)
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
        lprintf("len is not page alinged");
        return -4;
    }

    unsigned int base_addr = (unsigned int)addr;

    if (base_addr % PAGE_SIZE != 0)
    {
        lprintf("base is not page aligned");
        return -3;
    }

    if (base_addr < USER_MEM_START)
    {
        lprintf("User space trying to access kernel memory");
        return -2;
    }

    int num_pages = 0;
    for (unsigned int start = base_addr; start < base_addr + len; start += PAGE_SIZE)
    {
        void *frame_addr = get_frame_addr();
        if (frame_addr == NULL)
        {
            panic("No more pages left to assign");
        }
        if(add_frame(start, (unsigned int)frame_addr, (pde *)((void *)get_cr3()), USER_PD_FLAG, USER_PT_FLAG) < 0){
            return -1;
        }
        num_pages++;
    }
    lprintf("address %d pages at address %x", num_pages, (unsigned int)addr);
    return 0;
}

int align_pages(void *addr, int size)
{
    unsigned int addr_aligned = (unsigned int)addr & PAGE_MASK;
    int size_aligned = size;

    if (size_aligned % PAGE_SIZE != 0)
        size_aligned = (size_aligned / PAGE_SIZE) * PAGE_SIZE + PAGE_SIZE;

    return new_pages((void *)addr_aligned, size_aligned);
}

void *clone_page_directory(void *old_pd)
{
    assert(old_pd != NULL);
    void *start_map = (void *)USER_MEM_START;
    pde *copy_to = create_new_pd();
    pde *copy_from = (pde *)old_pd;
    int pages_copied = 0;
    while ((unsigned int)start_map < USER_MEM_END 
    && (unsigned int)start_map >= USER_MEM_START)
    {
        //MAGIC_BREAK;
        //lprintf("VA IS %p\n", start_map);
        unsigned int pd_idx = (unsigned int)get_pd_index((void *)start_map);
        if (!check_present((void *)copy_from[pd_idx]))
        {
            // start_map = start_map + (1 << 22);
            //lprintf("Switching to new dir");
            start_map += (1 << 22);
            continue;
            // increment by directory
        }
        pte *pt_start = (pte *)(copy_from[pd_idx] & CLEAR_BOTTOM);
        int pt_idx = (unsigned int)get_pt_index((void *)start_map);
        if (!check_present((void *)pt_start[pt_idx]))
        {
            // start_map = start_map + (1 << 12);
            //lprintf("Switching to new page");
            start_map += PAGE_SIZE;
            continue;
            // increment to next page
        }
        void *new_frame = get_frame_addr();
        if (add_frame((unsigned int)start_map, (unsigned int)new_frame,
         copy_to, USER_PD_FLAG, USER_PT_FLAG) < 0)
        {
            lprintf("VA FAIL: %x, PHYS FAIL: %x", (unsigned int)start_map, (unsigned int)new_frame);
            panic("add frame did not work");
        } // should copy the actual flags but this should be fine for now
        // memcpy(new_frame, start_map, PAGE_SIZE); 
        //CHECK IF THIS WORKS V POSSIBLE IT DOESN'T
        lprintf("yay new page has been mapped now VA ADDY: %x",
        (unsigned int)start_map);
        pages_copied++;
        start_map += PAGE_SIZE;
    }
    lprintf("WE copied %d pages", pages_copied);
    map_kernel_space(copy_to);
    return (void *)copy_to;
}