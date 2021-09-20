#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ssize_t read_line(int fd, void *buffer, size_t n) {
    if (n == 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    char *buf = buffer;
    ssize_t total = 0;
    while (1) {
        errno = 0;

        char ch;
        ssize_t num_read = read(fd, &ch, 1);
        if (num_read == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        } else if (num_read == 0) {
            // read EOF
            if (total == 0) {
                fprintf(stderr, "## check\n");
                return 0;
            }

            break;
        } else {
            if (total < n - 1 && ch != '\n') {
                *buf = ch;
                ++buf;
                ++total;
            }

            if (ch == '\n') {
                break;
            }
        }
    }

    *buf = '\0';
    return total;
}

int main(int argc, char *argv[]) {
    int fd;
    if (argc < 2) {
        fd = STDIN_FILENO;
    } else {
        fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("open");
            return -1;
        }
    }

    char buffer[32];
    ssize_t n = read_line(fd, buffer, 32);
    assert(strcmp(buffer, "foo") == 0);
    assert(n == 3);

    n = read_line(fd, buffer, 32);
    assert(strcmp(buffer, "apple orange") == 0);
    assert(n == 12);

    n = read_line(fd, buffer, 32);
    assert(strcmp(buffer, "baz") == 0);
    assert(n == 3);

    printf("OK\n");
    return 0;
}