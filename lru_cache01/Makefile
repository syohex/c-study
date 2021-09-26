all: lru_cache.a

lru_cache.a: lru_cache.c
	$(CC) -Wall -g -c -o lru_cache.o lru_cache.c
	$(AR) rcs lru_cache.a lru_cache.o

lru_cache_test: test.c lru_cache.a
	$(CC) -Wall -g -o lru_cache_test test.c lru_cache.a

.PHONY: test
test: lru_cache_test
	./lru_cache_test

.PHONY: clean
clean:
	-rm -f lru_cache.a test
