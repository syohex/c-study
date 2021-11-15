CFLAGS=-Wall -fsanitize=address,undefined

all: tcp_server01

tcp_server01: main.c
	$(CC) $(CFLAGS) -o $@ $^