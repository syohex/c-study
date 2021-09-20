all: read_line

read_line: main.c
	$(CC) -Wall -o read_line main.c

.PHONY: test clean
test: read_line
	./read_line < test_data

clean:
	-rm -f *.o read_line
