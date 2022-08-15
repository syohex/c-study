#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        ssize_t len = tee(STDIN_FILENO, STDOUT_FILENO, INT_MAX, SPLICE_F_NONBLOCK);
        if (len < 0) {
            if (errno == EAGAIN) {
                continue;
            }

            perror("tee");
            return 1;
        } else if (len == 0) {
            break;
        }

        while (len > 0) {
            ssize_t slen = splice(STDIN_FILENO, NULL, fd, NULL, len, SPLICE_F_MOVE);
            if (slen < 0) {
                perror("splice");
                break;
            }

            len -= slen;
        }
    }

    (void)close(fd);
    return 0;
}