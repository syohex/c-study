#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PORT_NUM 50003

int main(int argc, char *argv[]) {
    int sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(struct sockaddr_in6));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(PORT_NUM);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in6)) == -1) {
        perror("bind");
        return -1;
    }

    // receive message
    while (1) {
        char buffer[32];
        struct sockaddr_in6 client;
        socklen_t len = sizeof(struct sockaddr_in6);
        char addr_buf[INET6_ADDRSTRLEN];

        ssize_t n_bytes = recvfrom(sock, buffer, 32, 0, (struct sockaddr *)&client, &len);
        if (n_bytes == -1) {
            perror("recvfrom");
            return -1;
        }

        if (inet_ntop(AF_INET6, &client.sin6_addr, addr_buf, INET6_ADDRSTRLEN) == NULL) {
            printf("Could not convert client address to string\n");
        } else {
            printf("Server receive %ld bytes from (%s, %u)\n", n_bytes, addr_buf, ntohs(client.sin6_port));
        }

        for (ssize_t i = 0; i < n_bytes; ++i) {
            buffer[i] = toupper((int)buffer[i]);
        }

        if (sendto(sock, buffer, n_bytes, 0, (struct sockaddr *)&client, len) == -1) {
            perror("sendto");
            return -1;
        }
    }

    return 0;
}