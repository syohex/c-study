#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

static const long DEFAULT_LINES = 10;

static struct option longopts[] = {
    {"lines", required_argument, NULL, 'n'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0},
};

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
    int opt;
    long lines = DEFAULT_LINES;

    while ((opt = getopt_long(argc, argv, "n:", longopts, NULL)) != -1) {
        switch (opt) {
        case 'n':
            lines = atol(optarg);
            break;
        case 'h':
            printf("Usage: %s [-n lines] [-h] file1 ...\n", argv[0]);
            exit(0);
        case '?':
            printf("Usage: %s [-n lines] [-h] file1 ...\n", argv[0]);
            exit(1);
        }
    }

    if (optind == argc) {
        do_head(stdin, lines);
    } else {
        for (int i = optind; i < argc; ++i) {
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