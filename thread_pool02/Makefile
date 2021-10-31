FILES=job_queue.c semaphore.c thread_pool.c thread.c

all: example

example: example.c $(FILES)
	$(CC) -Wall -g -fsanitize=address,undefined -o example example.c $(FILES) -lpthread

.PHONY: format clean
format:
	clang-format -i *.c *.h

clean:
	-rm -f example *.o