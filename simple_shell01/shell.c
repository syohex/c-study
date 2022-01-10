#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *lsh_read_line(void) {
    static const size_t LINE_BUF_SIZE = 256;
    size_t buf_size = LINE_BUF_SIZE;

    char *buffer = malloc(buf_size * sizeof(char));
    if (buffer == NULL) {
        perror("malloc");
        exit(1);
    }

    size_t pos = 0;
    while (true) {
        int c = getchar();
        if (c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        }

        buffer[pos] = c;
        ++pos;

        if (pos >= buf_size) {
            buf_size += LINE_BUF_SIZE;
            buffer = realloc(buffer, buf_size);
            if (buffer == NULL) {
                perror("realloc");
                exit(1);
            }
        }
    }

    // never reach here
    return NULL;
}

char **lsh_split_line(char *line) {
    static const size_t TOKEN_BUF_SIZE = 128;
    static const char *const TOKEN_DELIMITER = " \t\r\n\a";

    size_t buf_size = TOKEN_BUF_SIZE;
    char **tokens = malloc(buf_size * sizeof(char *));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    char *token = strtok(line, TOKEN_DELIMITER);
    size_t pos = 0;
    while (token != NULL) {
        tokens[pos] = token;
        ++pos;

        if (pos >= buf_size) {
            buf_size += TOKEN_BUF_SIZE;
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

static int lsh_launch(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // parent
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    int status;
    do {
        waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}

int builtin_cd(char **args) {
    char *dir;
    if (args[1] == NULL) {
        dir = getenv("HOME");
    } else {
        dir = args[1];
    }

    if (chdir(dir) == -1) {
        perror("chdir");
    }

    return 0;
}

int builtin_exit(char **args) {
    return 1;
}

struct BuiltinFunction {
    char *name;
    int (*func)(char **args);
};

static struct BuiltinFunction builtin_functions[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
};

int lsh_execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    const size_t builtins = sizeof(builtin_functions) / sizeof(struct BuiltinFunction);
    for (size_t i = 0; i < builtins; ++i) {
        if (strcmp(args[0], builtin_functions[i].name) == 0) {
            return builtin_functions[i].func(args);
        }
    }

    return lsh_launch(args);
}

int main(int argc, char *argv[]) {
    while (true) {
        printf("> ");
        fflush(stdout);

        char *line = lsh_read_line();
        char **args = lsh_split_line(line);
        int status = lsh_execute(args);

        free(line);
        free(args);

        if (status != 0) {
            return 1;
        }
    }

    return 0;
}
