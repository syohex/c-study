#include <stdio.h>
#include <stdlib.h>

static const int LINE_BUF_SIZE = 256;
static const char *const TOKEN_DELIMITER = " \t\r\n\a";

char *lsh_read_line(void) {
    int buf_size = LINE_BUF_SIZE;

}

char **lsh_split_line(char *line) {
    size_t buf_size = LINE_BUF_SIZE;
    char **tokens = malloc(buf_size * sizeof(char *));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    char *token = strtok(line);
    size_t pos = 0;
    while (token != NULL) {
        tokens[pos] = token;
        ++pos;

        if (pos >= buf_size) {
            buf_size += LINE_BUF_SIZE;
            tokens = realloc(tokens, buf_size * sizeof(char *));
            if (tokens == NULL) {
                perror("realloc");
                exit(1);
            }
        }

        token = strtok(NULL, TOKEN_DELIMITER);
    }

    tokens[pos] = NULL;
    return tokens;
}

static lsh_read_line(void) {
}

static void lsh_loop(void) {
}

int main(int argc, char *argv[]) {
    return 0;
}
