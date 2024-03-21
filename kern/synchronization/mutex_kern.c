#include <stdlib.h>
#include <synchronization/list_helper.h>

mutex_t* mutex_init(){
    mutex_t* ret_obj = malloc(sizeof(mutex_t));
    if(ret_obj){
        lprintf("malloc didn't work");
        return NULL;
    }
    ret_obj->locked = 0;
    if(queue_init(&ret_obj->wait_queue) < 0){
        lprintf("Could not initialize mutex wait queue");
        return NULL;
    }
    return ret_obj;
}

int mutex_lock(int tid, mutex_t* mutex){
    assert(mutex != NULL);
    // Stop context switching
    if(!mutex->locked){
        mutex->locked = 1;
        // enable context switching
    }else{
        add_queue(tid, &mutex->wait_queue);
        // disable interruprts
        // enable context switchiing
        // put ourselves on sleep que


        // Put ourselves on sleep queue and wake up when unlocking
        // enable context switching
        // ^^ need a way to do this atomically
    }
    return 0;
}

int mutex_unlock(mutex_t* mutex){
    assert(mutex != NULL);
    // stop context switching
    if(queue_is_empty(&mutex->wait_queue)){
        mutex->locked = 0;
        // resume context switching
    }else{
        int switch_tid = remove_queue(&mutex->wait_queue);
        // remove this person from the sleep queue
        // enable context switching
        // ^^ need to also do this atomically
    }
    return 0;
}

int mutex_destroy(mutex_t* mutex){
    assert(mutex != NULL && queue_is_empty(mutex->wait_queue));
    int err_val = queue_destroy(&mutex->wait_queue);
    free(mutex);
    return err_val;
}
