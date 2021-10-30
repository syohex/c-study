CFLAGS=-Wall -fsanitize=address,undefined -g
LDLIBS=-lpthread

all: pool_test

thread_pool.o: thread_pool.c
	$(CC) $(CFLAGS) -c -o thread_pool.o thread_pool.c

pool_test: pool_test.c thread_pool.o
	$(CC) $(CFLAGS) -o pool_test pool_test.c thread_pool.o $(LDLIBS)
