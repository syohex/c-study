#include <sys/eventfd.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s num...\n", argv[0]);
        return 1;
    }

    int efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        for (int i = 1; i < argc; ++i) {
            printf("child writing %s to efd\n", argv[i]);
            unsigned long long num = strtoull(argv[i], NULL, 0);
            ssize_t len = write(efd, &num, sizeof(unsigned long long));
            if (len != sizeof(unsigned long long)) {
                perror("write");
                return 1;
            }
        }

        printf("Child finished\n");
        return 0;
    }

    sleep(3);

    printf("Parent read eventfd\n");
    unsigned long long num = 0;
    ssize_t len = read(efd, &num, sizeof(unsigned long long));
    if (len != sizeof(unsigned long long)) {
        perror("read");
        return 1;
    }

    printf("parent read %llu\n", num);
    return 0;
}
