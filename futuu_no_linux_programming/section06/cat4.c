#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    char buf[1024];
    for (int i = 1; i < argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            perror("fopen");
            exit(1);
        }

        while (true) {
            size_t n = fread(buf, 1, 1024, fp);
            if (ferror(fp)) {
                perror("fread");
                exit(1);
            }

            if (fwrite(buf, 1, n, stdout) == 0) {
                perror("fwrite");
                exit(1);
            }

            if (feof(fp)) {
                break;
            }
        }

        fclose(fp);
    }

    return 0;
}