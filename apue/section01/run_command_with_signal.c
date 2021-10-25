#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static void print_prompt(void) {
    printf("> ");
    fflush(stdout);
}

static void sigint_handler(int sig) {
    printf("got SIGINT\n"); // this is not signal safe :-(
}

int main(void) {
    char buffer[256];

    signal(SIGINT, sigint_handler);

    print_prompt();
    while (1) {
        if (fgets(buffer, 256, stdin) == NULL) {
            perror("fgets");
            return 1;
        }

        size_t len = strlen(buffer);
        buffer[len - 1] = '\0';

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            // child process
            execlp(buffer, buffer, NULL);
            // never reach here
            return 1;
        }

        // parent
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        printf("## exit status=%d\n", status);

        print_prompt();
    }

    return 0;
}