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
#include <memory/hash_helper.h>
#include <inc/asm_helpers.h>

hashtable hash_tb;
frame_node_t *head = NULL;

void *frame_curr = (void *)USER_MEM_START;

// Top
void *get_physical_address(void *pt_entry)
{
    return (void *)((unsigned int)pt_entry & ~TWELVE_BIT_MASK);
    // This needs to be a 12 bit mask, check handout
}

// This makes sense right??
void set_not_present(pte pt_entry)
{
    pt_entry = (pte)(0);
    return;
}

/// @brief
/// @param entry
/// @return
int get_pt_index(void *entry)
{
    return (((unsigned int)entry >> 12) & TEN_BIT_MASK);
}

/// @brief
/// @param entry
/// @return
int get_pd_index(void *entry)
{
    return (((unsigned int)entry >> 22) & TEN_BIT_MASK);
}

/// @brief
/// @param entry
/// @return
int check_present(void *entry)
{
    return ((unsigned int)entry & PRESENT_BIT_MASK);
}

/// @brief
/// @param entry
/// @param flags
/// @return
unsigned int set_flags(void *entry, int flags)
{
    return ((unsigned int)entry | flags);
}

int check_addr_present(void *virtual_address)
{
    pde *pd_start = (pde *)get_cr3();
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    pte *pt_start = (pte *)(pd_start[pd_idx] & CLEAR_BOTTOM);
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);

    // void *page_table = (void *)(*(unsigned int *)(pd_start + pd_idx));
    return check_present((void *)pt_start[pt_idx]) && (unsigned int)virtual_address != (unsigned int)COPY_ADDR_VA;
}

/// @brief
/// @return
pde *create_new_pd()
{
    pde *pd_new = smemalign(PAGE_SIZE, PAGE_SIZE);
    // Allocating array of 1024 unsigned ints
    // pde *pd_new = smalloc(PAGE_SIZE);
    assert(pd_new);
    memset(pd_new, 0, PAGE_SIZE); // Setting all to 0 in the start
    return pd_new;
}

void *get_physical(unsigned int virtual_address, pde *pd_start)
{
    assert(pd_start != NULL);
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    if (!check_present((void *)pd_start[pd_idx]))
    {
        lprintf("trying to get PA of a page whose directory doesn't exist");
        return NULL;
    }

    pte *pt_start = (pte *)(pd_start[pd_idx] & CLEAR_BOTTOM);
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);

    if (!check_present((void *)pt_start[pt_idx]))
    {
        lprintf("trying to get PA of a page with non present page");
        return NULL;
    }

    void *ret_PA = get_physical_address((void *)pt_start[pt_idx]);

    return ret_PA;
}

void *remove_frame(unsigned int virtual_address, pde *pd_start)
{
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

    void *free_frame = get_physical_address((void *)pt_start[pt_idx]);
    set_not_present(pt_start[pt_idx]);

    return free_frame;
}

// Need to check for flags
int add_frame(unsigned int virtual_address, unsigned int physical_address,
              pde *pd_start, int pd_flags, int pt_flags)
{
    unsigned int pd_idx = (unsigned int)get_pd_index((void *)virtual_address);
    // Check if the pd_start[pd_idx] is present otherwise create
    // Check if pd_start[pd_idx][pt_idx] is present otherwise create
    // Once both and creating then store frame in pd_start[pd_idx][pt_idx]
    // Need to read about the bottom 12 bit flags associated with both entries
    if (!check_present((void *)pd_start[pd_idx]))
    {
        pde *new_page_table = create_new_pd();
        // new_page_table = (pde *)set_flags(new_page_table, pd_flags);
        pd_start[pd_idx] = set_flags(new_page_table, pd_flags);
    }
    pte *pt_start = (pte *)(pd_start[pd_idx] & CLEAR_BOTTOM);
    int pt_idx = (unsigned int)get_pt_index((void *)virtual_address);

    // void *page_table = (void *)(*(unsigned int *)(pd_start + pd_idx));
    if (check_present((void *)pt_start[pt_idx]) && virtual_address != (unsigned int)COPY_ADDR_VA)
    {
        lprintf("page already seems to be mapped %x\n", pt_start[pt_idx]);
        return -1;
    }
    if (virtual_address == (unsigned int)COPY_ADDR_VA)
    {
        lprintf("Created a mapping to PHYS space %x", physical_address);
    }
    pt_start[pt_idx] = (pte)(physical_address | pt_flags);
    return 0;
}

/// @brief
/// @param pd_start
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

/// @brief
void initialize_vm()
{
    pde *pd_start = create_new_pd();
    init_hashtable(&hash_tb);
    map_kernel_space(pd_start);
    set_zero_page();
    set_cr3((unsigned int)pd_start);
    set_cr0(get_cr0() | CR0_PG | CR0_PE);
    // Need to zero out ZFOF frame here also
    // How to do, do we just assign a VA to it, use memset and then clear the page
}

/// @brief
/// @return
void *get_frame_addr()
{
    void *ret_frame;
    ret_frame = get_free_frame(head);
    if (ret_frame != NULL)
    {
        return ret_frame;
    }
    if(frame_curr == ZFOD_ADDR_PA){
        frame_curr += PAGE_SIZE;
    }
    ret_frame = frame_curr;
    frame_curr += PAGE_SIZE;
    if ((unsigned int)ret_frame > machine_phys_frames() * PAGE_SIZE)
    {
        lprintf("machine oom %p\n", ret_frame);
        return NULL;
    }
    return ret_frame;
}

void set_zero_page(){
    add_frame(USER_MEM_START, (unsigned int)ZFOD_ADDR_PA,
              (pde *)((void *)get_cr3()), USER_PD_FLAG, USER_PT_FLAG);
    memset(USER_MEM_START, 0, PAGE_SIZE);
    remove_frame(USER_MEM_START, (pde *)((void *)get_cr3()));
}

/// @brief
/// @param addr
/// @param len
/// @return
int new_pages(void *addr, int len)
{

    assert(addr != NULL);
    lprintf("new page addr %p %d\n", addr, check_present(addr));
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

    if (check_present(addr))
        return -7;

    int num_pages = 0;

    // for (unsigned int start = base_addr; start < base_addr + len;
    //      start += PAGE_SIZE)
    // {
    //     if (check_addr_present((void *)start))
    //         return -1;
    // }

    for (unsigned int start = base_addr; start < base_addr + len;
         start += PAGE_SIZE)
    {
        void *frame_addr = ZFOD_ADDR_PA;
        // if (frame_addr == NULL)
        // {
        //     // panic("No more pages left to assign");
        //     return -5;
        // }
        if (add_frame(start, (unsigned int)frame_addr,
                      (pde *)((void *)get_cr3()), USER_PD_FLAG, USER_PT_FLAG_INITIAL) < 0)
        {
            lprintf("page already mapped, shouldnt reach here\n");
            return -1;
        }
        num_pages += 1;
    }
    // lprintf("added page from %p to %p\n", addr, addr + len);
    vm_hash_node_t *new_node = malloc(sizeof(vm_hash_node_t));
    if (new_node == NULL)
    {
        return -8;
    }
    new_node->addr = addr;
    new_node->num_pages = num_pages;
    insert_addr_thread(new_node, &hash_tb); // insert virtual address with the amount of pages it took also
    return 0;
}

// Hashtable needs to contain virtual address -> len mapping
int remove_pages(void *addr)
{
    assert(addr != NULL);
    if ((unsigned int)addr < USER_MEM_START)
    {
        // Trying to remove kernel pages
        return -1;
    }
    vm_hash_node_t *retrieve_node = get_thread((unsigned int)addr, &hash_tb);
    int num_pages = retrieve_node->num_pages;
    // void* start_addr = retrieve_node->addr;
    remove_addr_thread(retrieve_node, &hash_tb); // Remove thread from hash_tb
    unsigned int start = (unsigned int)addr;
    for (unsigned int start_addr = start; start_addr < (start + (PAGE_SIZE * num_pages)); start_addr += PAGE_SIZE)
    {
        void *frame_addr = remove_frame(start_addr, (pde *)((void *)(get_cr3)));
        if((unsigned int)frame_addr == ZFOD_ADDR_PA){
            continue;
        }
        add_free_frame(frame_addr, head);
    }
    return 0;
}

/// @brief
/// @param addr
/// @param size
/// @return
int align_pages(void *addr, int size)
{
    unsigned int addr_aligned = (unsigned int)addr & PAGE_MASK;
    int size_aligned = size;

    if (size_aligned % PAGE_SIZE != 0)
        size_aligned = (size_aligned / PAGE_SIZE) * PAGE_SIZE + PAGE_SIZE;

    return new_pages((void *)addr_aligned, size_aligned);
}

/// @brief Clones a directory
/// @param old_pd
/// @return
void *clone_page_directory(void *old_pd)
{
    assert(old_pd != NULL);
    void *start_map = (void *)USER_MEM_START;
    pde *copy_to = create_new_pd();
    pde *copy_from = (pde *)old_pd;
    int pages_copied = 0;
    assert(get_cr3() == (uint32_t)old_pd);
    while ((unsigned int)start_map < USER_MEM_END && (unsigned int)start_map >= USER_MEM_START)
    {
        // MAGIC_BREAK;
        // lprintf("VA IS %p\n", start_map);
        unsigned int pd_idx = (unsigned int)get_pd_index((void *)start_map);
        if (!check_present((void *)copy_from[pd_idx]))
        {
            start_map += (1 << 22);
            continue;
            // increment by directory
        }
        pte *pt_start = (pte *)(copy_from[pd_idx] & CLEAR_BOTTOM);
        int pt_idx = (unsigned int)get_pt_index((void *)start_map);
        if (!check_present((void *)pt_start[pt_idx]))
        {
            start_map += PAGE_SIZE;
            continue;
            // increment to next page
        }

        unsigned int clone_PA = get_physical(start_map, copy_from);
        
        if(clone_PA == ZFOD_ADDR_PA){
            if (add_frame(clone_PA, ZFOD_ADDR_PA,
                        copy_to, USER_PD_FLAG, USER_PT_FLAG_INITIAL) < 0)
            {
                lprintf("page already mapped, shouldnt reach here\n");
                return -1;
            }
        }
        else{
            void *new_frame = get_frame_addr();
            // CHECK IF THIS WORKS V POSSIBLE IT DOESN'T
            lprintf("yay new page has been mapped now VA ADDY: %x", (unsigned int)start_map);
            if (add_frame((unsigned int)COPY_ADDR_VA, (unsigned int)new_frame,
                        copy_from, USER_PD_FLAG, USER_PT_FLAG) < 0)
            {
                lprintf("Something wrong happened here");
            }
            flush_page_entry((unsigned int)COPY_ADDR_VA);
            memmove((void *)COPY_ADDR_VA, (void *)start_map, PAGE_SIZE);
            if (add_frame((unsigned int)start_map, (unsigned int)new_frame,
                        copy_to, USER_PD_FLAG, USER_PT_FLAG) < 0)
            {
                lprintf("VA FAIL: %x, PHYS FAIL: %x",
                        (unsigned int)start_map, (unsigned int)new_frame);
                panic("add frame did not work");
            } // should copy the actual flags but this should be fine for now
        }
        pages_copied++;
        start_map += PAGE_SIZE;
    }
    // lprintf("WE copied %d pages", pages_copied);
    map_kernel_space(copy_to);
    return (void *)copy_to;
}

int in_user_mem(unsigned int addr)
{
    if (addr < USER_MEM_END)
        return -1;
    else
        return 0;
}

/* Check present and supervisor flags */
int check_vaddr(void *addr)
{
    if (in_user_mem((unsigned int)addr) < 0)
    {
        return -1;
    }
    // unsigned int page = addr & PAGE_MASK;
    int flags = PRESENT_BIT_MASK | SUPERVISOR_BIT_MASK; // Address does not need to be writable
    pde *pd = (pde *)get_cr3();
    // if (pd & PRESENT_BIT_MASK)
    //{
    pte *page_table = (pte *)pd[get_pd_index(addr)];
    if (page_table[get_pt_index(addr)] & flags)
        return 0;
    //}

    return -1;
}

int validate_string(char *string)
{
    // assuming all strings end with null terminator
    int len = 0;
    while (check_vaddr(string + len) == 0)
    {
        if (string[len] == '\0')
            return len;
        len++;
    }
    return -1;
}

int validate_string_array(char *string_arr[])
{
    int len = 0;
    while (check_vaddr(string_arr + len) == 0)
    {
        if (string_arr[len] == NULL)
            return len;

        if (validate_string(string_arr[len]) >= 0)
            len++;
        else
            return -1;
    }
    return -2;
}