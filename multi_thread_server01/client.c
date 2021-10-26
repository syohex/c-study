#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    uint16_t port;
    if (argc < 2) {
        port = DEFAULT_PORT;
    } else {
        port = atoi(argv[1]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("## connected\n");

    char buffer[256];
    if (argc >= 3) {
        strcpy(buffer, argv[2]);
    } else {
        fgets(buffer, 256, stdin);
        size_t len = strlen(buffer);
        buffer[len - 1] = '\0';
    }

    size_t buf_len = strlen(buffer);
    printf("## send\n");
    if (write(sock, buffer, buf_len) == -1) {
        perror("send");
        close(sock);
        return 1;
    }

    char recv_buffer[256];
    if (recv(sock, recv_buffer, 256, 0) == -1) {
        perror("recv");
        close(sock);
        return 1;
    }

    printf("%s\n", recv_buffer);
    close(sock);
    return 0;
}