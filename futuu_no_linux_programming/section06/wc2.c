#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: wc2 file1 file2 ...\n");
        exit(1);
    }

    for (int i = 1; i < argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            perror("fopen");
            exit(1);
        }

        int c;
        int lines = 0;
        while ((c = fgetc(fp)) != EOF) {
            if (c == '\n') {
                ++lines;
            }
        }

        printf("%5d %s\n", lines, argv[i]);

        fclose(fp);
    }

    return 0;
}