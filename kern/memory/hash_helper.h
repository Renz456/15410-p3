#ifndef HASH_HELPER_H
#define HASH_HELPER_H

#define SIZE_HASH 1000

// Define the structure for the hashtable
typedef struct hashtable
{
    int items;
    vm_hash_node_t *table[SIZE_HASH]; // An array of pointers to nodes (linked list heads)
} hashtable;

typedef struct vm_hash_node_s
{
    void* addr;
    int num_pages;
    struct vm_hash_node_s *next;
    struct vm_hash_node_s *prev;
} vm_hash_node_t;

typedef struct frame_node_s
{
    void *frame_addr;
    struct frame_node_s *next;
} frame_node_t;

void free_table(hashtable *hash_tb);

int init_hashtable(hashtable *hash_tb);

int hash_func(unsigned int key);

void insert_thread(vm_hash_node_t *thread, hashtable *hash_tb);

void remove_thread(vm_hash_node_t *thread, hashtable *hash_tb);

vm_hash_node_t *get_thread(unsigned int addr, hashtable *hash_tb);


#endif /* HASH_HELPER_H */
