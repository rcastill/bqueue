#include "bqueue.h"
#include <stdlib.h>

int bqueue_init(bqueue_t *b)
{
    b->front = 0;
    b->back = 0;
    b->size = 0;
    int pthread_r = 0;
    if ((pthread_r = pthread_mutex_init(&b->mutex, 0)) != 0)
    {
        return pthread_r;
    }
    if ((pthread_r = pthread_cond_init(&b->cond, 0)) != 0)
    {
        return pthread_r;
    }
    return 0;
}

int bqueue_push(bqueue_t *b, void *data)
{
    int pthread_r = 0;
    if ((pthread_r = pthread_mutex_lock(&b->mutex)) != 0)
    {
        return pthread_r;
    }
    struct bqueue_node *n = malloc(sizeof(*n));
    n->data = data;
    n->next = 0;
    n->prev = b->back;
    if (b->back != 0)
    {
        b->back->next = n;
        b->back = n;
    }
    else
    {
        b->front = b->back = n;
    }
    b->size = b->size + 1;
    if ((pthread_r = pthread_cond_signal(&b->cond)) != 0)
    {
        return pthread_r;
    }
    if ((pthread_r = pthread_mutex_unlock(&b->mutex)) != 0)
    {
        return pthread_r;
    }
    return 0;
}

int bqueue_pop(bqueue_t *b, void **data)
{
    int pthread_r = 0;
    if ((pthread_r = pthread_mutex_lock(&b->mutex)) != 0)
    {
        return pthread_r;
    }
    while (b->front == 0)
    {
        if ((pthread_r = pthread_cond_wait(&b->cond, &b->mutex)) != 0)
        {
            // POSIX guarantees EINTR will not be returned
            return pthread_r;
        }
    }
    struct bqueue_node *front = b->front;
    b->front = front->next;
    if (b->front != 0)
    {
        b->front->prev = 0;
    }
    else
    {
        b->back = 0;
    }
    *data = front->data;
    free(front);
    b->size = b->size - 1;
    if ((pthread_r = pthread_mutex_unlock(&b->mutex)) != 0)
    {
        return pthread_r;
    }
    return 0;
}

int bqueue_destroy(bqueue_t* b)
{
    int pthread_r = 0;
    if ((pthread_r = pthread_mutex_lock(&b->mutex)) != 0) {
        return pthread_r;
    }
    while (b->front != 0) {
        struct bqueue_node *next = b->front->next;
        free(b->front);
        b->front = next;
    }
    if ((pthread_r = pthread_mutex_unlock(&b->mutex)) != 0) {
        return pthread_r;
    }
    if ((pthread_r = pthread_mutex_destroy(&b->mutex)) != 0) {
        return pthread_r;
    }
    if ((pthread_r = pthread_cond_destroy(&b->cond)) != 0) {
        return pthread_r;
    }
    return 0;
}

int bqueue_size(bqueue_t* b, unsigned long* size)
{
    int pthread_r = 0;
    if ((pthread_r = pthread_mutex_lock(&b->mutex)) != 0) {
        return pthread_r;
    }
    *size = b->size;
    if ((pthread_r = pthread_mutex_unlock(&b->mutex)) != 0) {
        return pthread_r;
    }
    return 0;
}