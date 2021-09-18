all: a.out

a.out: main.c
	$(CC) main.c -g $(shell pkg-config --cflags --libs gstreamer-1.0)

.PHONY: clean

clean:
	rm -f a.out
