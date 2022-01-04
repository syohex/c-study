all: cat wc

cat: cat.c
	gcc -Wall -fsanitize=address,undefined -o cat cat.c

wc: wc.c
	gcc -Wall -fsanitize=address,undefined -o wc wc.c

.PHONY: clean
clean:
	-rm -f cat
