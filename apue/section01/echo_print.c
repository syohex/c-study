#include <stdio.h>
#include <unistd.h>

int main(void) {
    char buffer[256];

    while (1) {
        ssize_t bytes = read(STDIN_FILENO, buffer, 256);
        if (bytes < 0) {
            perror("read");
            return 1;
        }
        if (bytes == 0) {
            break;
        }

        if (write(STDOUT_FILENO, buffer, bytes) < 0) {
            perror("write");
            return 1;
        }
    }
    return 0;
}