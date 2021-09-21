all: server client

server: server.c
	$(CC) -Wall -o server server.c

client: client.c
	$(CC) -Wall -o client client.c

.PHONY: clean
clean:
	-rm -f server client