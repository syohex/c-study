#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("user id=%d, group id=%d\n", (int)getuid(), (int)getgid());
    return 0;
}