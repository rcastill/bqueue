#ifndef BQUEUE_H
#define BQUEUE_H

#include <pthread.h>

struct bqueue_node
{
    struct bqueue_node *next;
    struct bqueue_node *prev;
    void *data;
};

typedef struct bqueue
{
    struct bqueue_node *front;
    struct bqueue_node *back;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} bqueue_t;

/*
 * Initializes blocking queue instance.
 * 
 * Returns 0 on success.
 * Returns error code on error.
 *  See pthread_{mutex,cond}_init
 */
int bqueue_init(bqueue_t* b);


/*
 * Pushes data into blocking queue.
 * 
 * Returns 0 on success.
 * Returns error code on error.
 *  See pthread_mutex_[un]lock
 */
int bqueue_push(bqueue_t* b, void* data);

/*
 * Pops data from blocking queue.
 * Blocks until data available.
 * 
 * Returns 0 on success and *data is set.
 * Returns error code on error and *data is not modified.
 *  See pthread_{mutex_[un]lock,cond_wait}
 */
int bqueue_pop(bqueue_t* b, void** data);

/*
 * Destroy instance.
 * Free all heap allocated nodes.
 * 
 * Returns 0 on success.
 * Returns error code on error.
 *  See pthread_{mutex,cond}_destroy
 */
int bqueue_destroy(bqueue_t* b);

#endif /* BQUEUE_H */