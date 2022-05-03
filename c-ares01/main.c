#include <ares.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

static void state_cb(void *data, int sock, int read, int write) {
    printf("Change state fd: %d, read: %d, write: %d\n", sock, read, write);
}

static void callback(void *arg, int status, int timeouts, struct hostent *host) {
    if (status != ARES_SUCCESS) {
        fprintf(stderr, "failed to lookup: %s\n", ares_strerror(status));
        return;
    }

    if (host == NULL) {
        fprintf(stderr, "host is null\n");
        return;
    }

    printf("Found address name %s\n", host->h_name);

    for (int i = 0; host->h_addr_list[i] != NULL; ++i) {
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(host->h_addrtype, host->h_addr_list[i], ip, sizeof(ip));
        printf("\tIP: %s\n", ip);
    }
}

static void wait_ares(ares_channel channel) {
    while (true) {
        fd_set read_fds, write_fds;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        int nfds = ares_fds(channel, &read_fds, &write_fds);
        if (nfds == 0) {
            break;
        }

        struct timeval tv;
        struct timeval *tvp = ares_timeout(channel, NULL, &tv);

        select(nfds, &read_fds, &write_fds, NULL, tvp);
        ares_process(channel, &read_fds, &write_fds);
    }
}

int main(int argc, char *argv[]) {
    const char *host = "google.com";
    if (argc >= 2) {
        host = argv[1];
    }

    int status = ares_library_init(ARES_LIB_INIT_ALL);
    if (status != ARES_SUCCESS) {
        fprintf(stderr, "ares_library_init: %s\n", ares_strerror(status));
        return 1;
    }

    struct ares_options options;
    options.sock_state_cb = state_cb;
    int optmask = ARES_OPT_SOCK_STATE_CB;

    ares_channel channel;
    status = ares_init_options(&channel, &options, optmask);
    if (status != ARES_SUCCESS) {
        fprintf(stderr, "ares_init_options: %s\n", ares_strerror(status));
        return 1;
    }

    ares_gethostbyname(channel, host, AF_INET, callback, NULL);
    wait_ares(channel);

    ares_destroy(channel);
    ares_library_cleanup();
    return 0;
}
