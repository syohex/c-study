#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("Current PID=%d\n", (int)getpid());
    return 0;
}