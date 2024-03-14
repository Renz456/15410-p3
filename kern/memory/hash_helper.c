/**
 * @file hash_helper.c
 * @author Abhinav Gupta (abhinav6)
 *         Rene Ravanan (rravanan)
 * @brief Contains helper functions to manipulate the hash table containing
 *        thread_t structs which store information about each thread and also
 *        the tcb_t struct which contains information about the previously freed pages
 *        and the current stack high and low.
 *
 * @bug Hashtable has arbitrarily chosen size. Does not dynamically resize with
 *      number of items
 */

#include <simics.h>
#include <stdio.h>
#include <hash_helper.h>
#include <assert.h>
#include <vm.h>

/**
 * @brief Frees the hashtable by passing all nodes thread_free_lock_destory
 *        and then freeing the hash table itself
 * @param hash_tb pointer to the hashtable which is to be freed
 */
void free_table(hashtable *hash_tb)
{
    for (int i = 0; i < SIZE_HASH; i++)
    {
        vm_hash_node_t *node = hash_tb->table[i];
        while (node != NULL)
        {
            vm_hash_node_t *next = node->next;
            // lprintf("FREEING NODE WITH TID %d\n", node->tid);
            free(node);
            node = next;
        }
    }
    // free(hash_tb->table);
}


/**
 * @brief Initializing the hash table
 *
 * @param hash_tb : pointer to the hashtbale
 * @return int : 0 on sucess and -1 on malloc error
 */
int init_hashtable(hashtable *hash_tb)
{
    // if ((hash_tb->table = calloc(SIZE_HASH, sizeof(thread_t *))) == NULL)
    // {
    //     return -1;
    // }
    for (int i = 0; i < SIZE_HASH; i++)
    {
        hash_tb->table[i] = NULL;
    }
    hash_tb->items = 0;
    return 0;
}

/**
 * @brief Hash function which returns the index into the hashtable given the key
 * @param key key of the node which is to be accessed
 * @return the index into the hashtable
 */
int hash_func(unsigned int key)
{
    return (key % SIZE_HASH);
}

/**
 * @brief Inserts a thread_t thread struct into the hashtable
 *
 * @param thread node which is to be inserted into the hashtable
 * @param hash_tb the hashtable where the node will be inserted
 */
void insert_thread(vm_hash_node_t *thread, hashtable *hash_tb)
{
    affirm(hash_tb != NULL);
    affirm(thread != NULL);

    int index = hash_func((unsigned int)thread->addr);
    thread->next = hash_tb->table[index];
    thread->prev = NULL;
    if (hash_tb->table[index] != NULL)
    {
        hash_tb->table[index]->prev = thread;
    }
    hash_tb->table[index] = thread;
    hash_tb->items += 1;
    return;
}

/**
 * @brief Inserts the inter thread struct into the hashtable
 *
 * @param thread : Pointer to the thread node to be inserted into the hashtable
 * @param hash_tb : Pointer to the Hashtable where the node is to be inserted
 */
void remove_thread(vm_hash_node_t *thread, hashtable *hash_tb)
{
    int index = hash_func((unsigned int)thread->addr);
    vm_hash_node_t *start = hash_tb->table[index];
    while (start != NULL)
    {
        if (start == thread)
        {
            if (start == hash_tb->table[index])
            {
                hash_tb->table[index] = hash_tb->table[index]->next;
            }
            else if (start->next == NULL)
            {
                start->prev->next = NULL;
            }
            else
            {
                start->prev->next = start->next;
                start->next->prev = start->prev;
            }
            return;
        }
        start = start->next;
    }
    return;
}

/**
 * @brief Retrives the thread note given its TID (key)
 *
 * @param tid : TID of the thread node which is to be found
 * @param hash_tb : Hashtable to search for TID object
 * @return thread_t* : Pointer to thread object, null if not found
 */
vm_hash_node_t *get_thread(unsigned int addr, hashtable *hash_tb)
{
    int index = hash_func(addr);
    vm_hash_node_t *start = hash_tb->table[index];
    while (start != NULL)
    {
        if ((unsigned int)start->addr == addr)
        {
            return start;
        }
        start = start->next;
    }
    return NULL;
}
