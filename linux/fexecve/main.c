#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    char *argv[] = {
        "sh",
        "-c",
        "echo $NAME",
        NULL,
    };
    char *environment[] = {
        "NAME=YamadaTaro",
        NULL,
    };

    int fd = open("/bin/sh", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fexecve(fd, argv, environment) < 0) {
        perror("fexecve");
        return 1;
    }

    printf("## never reach here\n");
    return 0;
}
