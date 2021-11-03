CFLAGS=-Wall -g -fsanitize=address,undefined
all: server

server: main.c
	$(CC) $(CFLAGS) -o server main.c -luv

.PHONY: clean
clean:
	-rm -f server *.o