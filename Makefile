CFLAGS=\
	-g\
	-Isrc
LFLAGS=\
	-pthread

all: test

test/bqueue.o: src/bqueue.c src/bqueue.h
	gcc $(CFLAGS) -c $< -o $@

test/test: test.c test/bqueue.o
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)

test: test/test
	./test/test
