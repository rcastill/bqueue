#include <bqueue.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void test1()
{
    printf("Running test 1\n");
    bqueue_t b;
    (void)bqueue_init(&b);
    int stack[10];
    for (int i = 0; i < 10; i++)
    {
        stack[i] = (i + 1) * 2;
        bqueue_push(&b, stack + i);
    }
    for (int i = 0; i < 10; i++)
    {
        int *k;
        (void)bqueue_pop(&b, (void **)&k);
        printf("k = %d\n", *k);
    }
    (void)bqueue_destroy(&b);
}

struct test2_consumer_args {
    int id;
    bqueue_t *b;
};

void *test2_consumer(void *arg)
{
    struct test2_consumer_args* args = (struct test2_consumer_args*) arg;
    int *k;
    while (1)
    {
        (void)bqueue_pop(args->b, (void **)&k);
        printf("[%d] k = %d\n", args->id, *k);
        if (*k == 0)
        {
            return 0;
        }
    }
}

void test2(int nthreads)
{
    printf("Running test 2: nthreads=%d\n", nthreads);
    bqueue_t b;
    (void)bqueue_init(&b);
    pthread_t* t = (pthread_t*)malloc(sizeof(*t) * nthreads);
    struct test2_consumer_args* argss = (struct test2_consumer_args*)malloc(sizeof(*argss)*nthreads);
    for (int i = 0; i < nthreads; i++) {
        argss[i].id = i;
        argss[i].b = &b;
        (void)pthread_create(t + i, 0, test2_consumer, (void*)(argss+i));
    }
    int stack[10];
    for (int i = 0; i < 10; i++)
    {
        (void)sleep(1);
        stack[i] = (i + 1) * 2;
        bqueue_push(&b, stack + i);
    }
    // nothing critical will happen if all access
    // the same &end
    int end = 0;
    // run nthreads times
    for (int i = 0; i < nthreads; i++) {
        (void)bqueue_push(&b, &end);
    }
    for (int i = 0; i < nthreads; i++){
        (void)pthread_join(t[i], 0);
    }
    free(t);
    free(argss);
    (void)bqueue_destroy(&b);
}

void* test3_consumer(void* a)
{
    bqueue_t* b = (bqueue_t*)a;
    int expecting_zero_to_exit = 0;
    unsigned long size;
    while (1)
    {
        (void)bqueue_size(b, &size);
        printf("size = %lu\n", size);
        // will exit after at least one non-zero
        // on next size==0
        if (expecting_zero_to_exit && size == 0)
        {
            return NULL;
        }
        else if (!expecting_zero_to_exit && size != 0)
        {
            expecting_zero_to_exit = 1;
        }
        sleep(1);
    }
}

void test3()
{
    printf("Running test 3\n");
    bqueue_t b;
    (void)bqueue_init(&b);
    pthread_t t;
    (void)pthread_create(&t, NULL, test3_consumer, &b);
    int stack[10];
    for (int i = 0; i < 10; i++)
    {
        stack[i] = (i + 1) * 2;
        bqueue_push(&b, stack + i);
        usleep(500000); // 500ms
    }
    for (int i = 0; i < 10; i++)
    {
        int *k;
        (void)bqueue_pop(&b, (void **)&k);
        usleep(500000); // 500ms
    }
    (void)pthread_join(t, NULL);
    (void)bqueue_destroy(&b);
}

int main(int argc, char const *argv[])
{
    test1();
    printf("\n");
    test2(1);
    printf("\n");
    test2(3);
    printf("\n");
    test3();
    return 0;
}
