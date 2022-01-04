#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

static size_t do_wc(int fd) {
    char buffer[4096];
    size_t lines = 0;
    while (true) {
        ssize_t n = read(fd, buffer, 4096);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }

            perror("read");
            continue;
        }
        if (n == 0) {
            break;
        }

        for (ssize_t i = 0; i < n; ++i) {
            if (buffer[i] == '\n') {
                ++lines;
            }
        }
    }

    return lines;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        size_t lines = do_wc(STDIN_FILENO);
        printf("%zd\n", lines);
    } else {
        size_t total = 0;
        for (int i = 1; i < argc; ++i) {
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(1);
            }

            size_t lines = do_wc(fd);
            printf("%5zd %s\n", lines, argv[i]);

            (void)close(fd);

            total += lines;
        }

        printf("%5zd total\n", total);
    }
    return 0;
}