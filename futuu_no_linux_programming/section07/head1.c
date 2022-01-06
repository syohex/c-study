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
        fprintf(stderr, "Usage: %s lines\n", argv[0]);
        exit(1);
    }

    long lines = atol(argv[1]);
    do_head(stdin, lines);
    return 0;
}