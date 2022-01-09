all: head1 head2 head3 tail1

head1: head1.c
	gcc -Wall -fsanitize=address,undefined -o head1 head1.c

head2: head2.c
	gcc -Wall -fsanitize=address,undefined -o head2 head2.c

head3: head3.c
	gcc -Wall -fsanitize=address,undefined -o head3 head3.c

tail1: tail1.c
	gcc -Wall -fsanitize=address,undefined -o tail1 tail1.c
