# bqueue

Standard and kept simple blocking queue for your every need.

## Test it works

```
make
```

## Include it in your project

```Makefile
bqueue.o: bqueue/src/bqueue.c bqueue/src/bqueue.h
    gcc -c $<

awesome-project: ... bqueue.o
    gcc -o awesome-project ... bqueue.o
```

...or just any variation of the above :D

## Use it

```c
#include <bqueue.h>

// Always initialize
bqueue_t b;
bqueue_init(&b);

// Produce some random stuff
// its up to you to manage that nasty void*
bqueue_push(&b, nasty_void_p);

// Consume some random stuff
bqueue_pop(&b, &nasty_void_p);

// Always clean
bqueue_destroy(&b);
```

Check out `test.c`, it works as an example.