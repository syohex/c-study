#define _GNU_SOURCE
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"

struct client_info {
    int sock;
    struct sockaddr_in addr;
};

static void *handler(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    char buffer[1024];

    printf("## Run thread %d\n", (int)gettid());

    while (1) {
        ssize_t len = recv(info->sock, buffer, 1024, 0);
        if (len < 0) {
            perror("recv");
            break;
        }
        if (len == 0) {
            break;
        }

        for (ssize_t i = 0; i < len; ++i) {
            buffer[i] = toupper(buffer[i]);
        }

        len = send(info->sock, buffer, len, 0);
        if (len < 0) {
            perror("send");
        }
    }

    close(info->sock);
    free(info);

    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    uint16_t port;
    if (argc < 2) {
        port = DEFAULT_PORT;
    } else {
        port = (uint16_t)atoi(argv[1]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    int enabled = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&enabled, sizeof(enabled)) == -1) {
        perror("setsockopt(SO_REUSEADDR)");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    printf("## Listen server port %u\n", port);

    if (listen(sock, 5) == -1) {
        perror("listen");
        return 1;
    }

    pthread_attr_t thread_attr;
    if (pthread_attr_init(&thread_attr) != 0) {
        perror("pthread_attr_init");
        return 1;
    }
    if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("pthread_attr_setdetachstate");
        return 1;
    }

    pthread_t thread;
    socklen_t info_size = sizeof(struct sockaddr_in);
    while (1) {
        struct client_info *info = (struct client_info *)malloc(sizeof(struct client_info));
        if (info == NULL) {
            perror("malloc(struct client_info)");
            return 1;
        }

        info->sock = accept(sock, (struct sockaddr *)&info->addr, &info_size);
        if (info->sock == -1) {
            perror("accept");
            free(info);
            continue;
        }

        if (pthread_create(&thread, &thread_attr, handler, (void *)info) != 0) {
            perror("pthread_create");
            free(info);
            continue;
        }
    }

    return 0;
}