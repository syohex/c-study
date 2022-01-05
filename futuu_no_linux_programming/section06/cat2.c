#include <stdio.h>
#include <stdlib.h>

static void do_cat(FILE *f) {
    int c;
    while ((c = fgetc(f)) != EOF) {
        fputc(c, stdout);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        do_cat(stdin);
    } else {
        for (int i = 1; i < argc; ++i) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror("fopen");
                exit(1);
            }

            do_cat(fp);
            fclose(fp);
        }
    }

    return 0;
}