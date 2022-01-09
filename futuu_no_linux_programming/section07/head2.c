#include <stdio.h>
#include <stdlib.h>

static void do_head(FILE *fp, long lines) {
    if (lines <= 0) {
        return;
    }

    int c;
    while ((c = getc(fp)) != EOF) {
        putchar(c);
        if (c == '\n') {
            --lines;
            if (lines == 0) {
                return;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        do_head(stdin, 10);
    } else {
        long lines = atol(argv[1]);
        for (int i = 2; i < argc; ++i) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror("fopen");
                exit(1);
            }

            do_head(fp, lines);
            fclose(fp);
        }
    }

    return 0;
}