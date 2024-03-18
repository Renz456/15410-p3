#include <stdlib.h>
#include <synchronization/list_helper.h>

int queue_init(mut_queue_t* queue){
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
}

int queue_destroy(mut_queue_t* queue){
    return 0;
}

int add_queue(int tid, mut_queue_t* queue){
    assert(queue != NULL);
    mut_node_t* new_node = malloc(sizeof(mut_node_t));
    if(new_node == NULL){
        return -1;
    }
    new_node->tid = tid;
    new_node->next = NULL;
    new_node->prev = NULL;
    
    if(queue->size == 0){
        queue->head = new_node;
        queue->tail = new_node;
    }
    else{
        assert(queue->size > 0);
        new_node->prev = queue->tail;
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size += 1;
    return 0;
}

int remove_queue(int tid, mut_queue_t* queue){
    assert(queue != NULL && queue->size > 0);
    int ret_val = queue->head->tid;
    mut_queue_t* free_node = queue->head;
    queue->head = queue->head->next;
    if(queue->head != NULL){
        queue->head->prev = NULL;
    }
    queue->size -= 1;
    free(free_node);
    return 0;
}

int queue_is_empty(mut_queue_t* queue){
    assert(queue != NULL);
    if(queue->size > 0){
        return 1;
    }
    return 0;
}