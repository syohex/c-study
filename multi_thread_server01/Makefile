OBJECTS=server client
CFLAGS=-Wall -fsanitize=address,undefined
LDFLAGS=-lpthread

all: $(OBJECTS)

.PHONY: clean
clean:
	-rm -f $(OBJECTS)
