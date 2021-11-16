CFLAGS=-Wall -fsanitize=address,undefined

all: tcp_server_chat

tcp_server_chat: main.c
	$(CC) $(CFLAGS) -o $@ $^
