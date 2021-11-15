CFLAGS=-Wall -g -fsanitize=address,undefined

all: interfaces

interfaces: main.c
	$(CC) $(CFLAGS) -o interfaces main.c
