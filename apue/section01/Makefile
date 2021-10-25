CFLAGS=-Wall -fsanitize=address,undefined
OBJECTS = \
	directory_entry

all: $(OBJECTS)

directory_entry: directory_entry.c
	$(CC) $(CFLAGS) -o directory_entry directory_entry.c

.PHONY: clean
clean:
	-rm -f $(OBJECTS)