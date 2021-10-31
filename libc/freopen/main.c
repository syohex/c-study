#include <stdio.h>

int main(void) {
    FILE *f = freopen("log.log", "a", stdout);
    if (f == NULL) {
        perror("freopen");
        return 1;
    }

    printf("hello world\n");
    fputs("hello bomber\n", f);
    return 0;
}
