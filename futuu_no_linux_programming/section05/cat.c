#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

static void do_cat(int fd) {
    char buf[4096];
    while (true) {
        ssize_t n = read(fd, buf, 4096);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("read");
            exit(1);
        }
        if (n == 0) {
            break;
        }

        if (write(STDOUT_FILENO, buf, n) == -1) {
            perror("write");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        do_cat(STDIN_FILENO);
    } else {
        for (int i = 1; i < argc; ++i) {
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "failed to open %s: %s\n", argv[i], strerror(errno));
                exit(1);
            }

            do_cat(fd);

            if (close(fd) == -1) {
                perror("close");
                exit(1);
            }
        }
    }

    return 0;
}