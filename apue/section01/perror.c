#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    errno = ENOMEM;
    perror(argv[0]);
    return 0;
}