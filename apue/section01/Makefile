CFLAGS=-Wall -fsanitize=address,undefined
OBJECTS = \
	directory_entry \
	echo_print \
	getpid \
	run_command \
	perror \
	uid_gid \
	run_command_with_signal

all: $(OBJECTS)

.PHONY: clean
clean:
	-rm -f $(OBJECTS)