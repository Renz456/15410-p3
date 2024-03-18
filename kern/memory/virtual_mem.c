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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <common_kern.h>
#include <cr.h>
#include <inc/kern_constants.h>
#include <vm.h>
#include <simics.h>
#include <memory/hash_helper.h>

hashtable hash_tb;
frame_node_t* head = NULL;

void *frame_curr = (void *)USER_MEM_START;

// Top 
void *get_physical_address(void *pt_entry){
    return (void *)((unsigned int)pt_entry & ~TWELVE_BIT_MASK); // This needs to be a 12 bit mask, check handout
}

// This makes sense right??
void set_not_present(pte pt_entry){
    pt_entry = (pte)(0);
    return;
}

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

void *remove_frame(unsigned int virtual_address, pde *pd_start){   
    assert(pd_start != NULL);
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    if (!check_present((void *)pd_start[pd_idx])) 
    {
        // Directory is not present
        lprintf("trying to remove page with non present directory");
        return NULL;
    }

    pte *pt_start = (pte *)(pd_start[pd_idx] & CLEAR_BOTTOM);
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);

    if (!check_present((void *)pt_start[pt_idx]))
    {
        lprintf("trying to remove page with non present page");
        return NULL;
    }

    void* free_frame = get_physical_address((void *)pt_start[pt_idx]);
    set_not_present((void *)pt_start[pt_idx]);

    return free_frame;
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
        lprintf("page already seems to be mapped %x\n", pt_start[pt_idx]);
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
    init_hashtable(&hash_tb);
    map_kernel_space(pd_start);
    set_cr3((unsigned int)pd_start);
    set_cr0(get_cr0() | CR0_PG | CR0_PE);
}

void *get_frame_addr()
{
    void *ret_frame;
    ret_frame = get_free_frame(head);
    if(ret_frame != NULL){
        return ret_frame;
    }
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
    int num_pages = 0;
    for (unsigned int start = base_addr; start < base_addr + len; start += PAGE_SIZE)
    {
        void *frame_addr = get_frame_addr();
        if (frame_addr == NULL)
        {
            panic("No more pages left to assign");
        }
        add_frame(start, (unsigned int)frame_addr, (pde *)((void *)get_cr3()), USER_PD_FLAG, USER_PT_FLAG);
        num_pages += 1;
    }
    // lprintf("added page from %p to %p\n", addr, addr + len);
    vm_hash_node_t *new_node = malloc(sizeof(vm_hash_node_t));
    if(new_node == NULL){
        return -1;
    }
    new_node->addr = addr;
    new_node->num_pages = num_pages;
    insert_thread(new_node, &hash_tb); // insert virtual address with the amount of pages it took also
    return 0;
}

// Hashtable needs to contain virtual address -> len mapping
int remove_pages(void *addr){
    assert(addr != NULL);
    if((unsigned int)addr < USER_MEM_START){
        // Trying to remove kernel pages
        return -1;
    }
    vm_hash_node_t *retrieve_node = get_thread((unsigned int)addr, &hash_tb);
    int num_pages = retrieve_node->num_pages;
    void* start_addr = retrieve_node->addr;
    remove_thread(retrieve_node, &hash_tb); //Remove thread from hash_tb
    unsigned int start = (unsigned int)addr;
    for(unsigned int start_addr = start; start_addr < (start + (PAGE_SIZE*num_pages)); start_addr += PAGE_SIZE){
        void* frame_addr = remove_frame(start_addr, (pde *)((void *)(get_cr3)));
        add_free_frame(frame_addr, head);
    }
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

// THIS NEEDS TO BE SQUASHED IN THE MERGE WITH CTX_SWITCH THIS IS WRONG
void *clone_page_directory(void *old_pd){
    assert(old_pd != NULL);
    void *start_map = (void *)USER_MEM_START;
    pde *copy_to = create_new_pd();
    pde *copy_from = (pde *)old_pd;
    while((unsigned int)start_map < 0xffffffff){
        unsigned int pd_idx = (unsigned int)get_pd_index((void *)start_map);
        if(!check_present((void *)copy_from[pd_idx])){
            start_map = start_map + (1 << 22);
            continue;
           // increment by directory
        }
        pte *pt_start = (pte *)(copy_from[pd_idx] & CLEAR_BOTTOM);
        int pt_idx = (unsigned int)get_pt_index((void *)start_map);
        if (!check_present((void *)pt_start[pt_idx]))
        {
            start_map = start_map + (1 << 12);
            continue;
            //increment to next page
        }
        void* new_frame = get_frame_addr();
        add_frame((unsigned int)start_map, (unsigned int)new_frame, copy_to, USER_PD_FLAG, USER_PT_FLAG); //should copy the actual flags but this should be fine for now
        memcpy(new_frame, start_map, PAGE_SIZE);
        start_map += PAGE_SIZE;
    }
    return (void *)copy_to;
}