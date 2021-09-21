#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PORT_NUM 50003

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("%s addr message...\n", argv[0]);
        return -1;
    }

    int sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(struct sockaddr_in6));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(PORT_NUM);
    if (inet_pton(AF_INET6, argv[1], &addr.sin6_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    for (int i = 2; i < argc; ++i) {
        char buffer[32];
        size_t len = strlen(argv[i]);
        if (sendto(sock, argv[i], len, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in6)) != len) {
            perror("sendto");
            return -1;
        }

        ssize_t n_bytes = recvfrom(sock, buffer, 32, 0, NULL, NULL);
        if (n_bytes == -1) {
            perror("recvfrom");
            return -1;
        }

        printf("## Response %d: %.*s\n", i - 1, (int)n_bytes, buffer);
    }

    return 0;
}