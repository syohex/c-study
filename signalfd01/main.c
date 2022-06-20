#include <sys/types.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }

    int sfd = signalfd(-1, &mask, 0);
    if (sfd == -1) {
        perror("signalfd");
        return 1;
    }

    printf("## wait signal\n");
    while (1) {
        struct signalfd_siginfo info;
        ssize_t len = read(sfd, &info, sizeof(info));
        if (len != sizeof(info)) {
            perror("read");
            return 1;
        }

        switch (info.ssi_signo) {
        case SIGINT:
            printf("GOT SIGINT\n");
            break;
        case SIGTERM:
            printf("GOT TERM\n");
            break;
        case SIGQUIT:
            printf("GOT QUIT and finish\n");
            return 0;
        default:
            printf("## unknown signal: %d\n", info.ssi_signo);
            break;
        }
    }

    return 0;
}