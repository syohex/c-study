all: snake

snake: main.cpp
	g++ -Wall -fsanitize=address,undefined -o snake main.cpp -lncurses

.PHONY: clean
clean:
	-rm -f *.out *.o snake