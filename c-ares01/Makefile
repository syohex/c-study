all: a.out

a.out: main.c
	gcc -Wall -g -fsanitize=address,undefined main.c -lcares

.PHONY: clean
clean:
	-rm -f a.out
