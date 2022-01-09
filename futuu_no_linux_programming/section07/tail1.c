#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tail1 file\n");
        return 1;
    }

    char *lines[10];
    for (int i = 0; i < 10; ++i) {
        lines[i] = malloc(4096 * sizeof(char));
    }

    FILE *fp = fopen(argv[1], "r");

    size_t index = 0;
    size_t pos = 0;
    int c;
    int line = 0;
    while ((c = fgetc(fp)) != EOF) {
        lines[index][pos] = c;
        ++pos;

        if (c == '\n') {
            ++line;

            lines[index][pos] = '\0';

            index = (index + 1) % 10;
            pos = 0;
        }
    }
    lines[index][pos] = '\0';

    fclose(fp);

    size_t start;
    if (line < 10) {
        start = 0;
    } else {
        start = index + 1;
    }

    for (size_t i = 0; i < 10; ++i) {
        size_t j = (start + i) % 10;
        printf("%s", lines[j]);
    }

    for (int i = 0; i < 10; ++i) {
        free(lines[i]);
    }

    return 0;
}