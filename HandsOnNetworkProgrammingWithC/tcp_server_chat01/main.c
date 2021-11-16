#define _POSIX_C_SOURCE 200112L // for vscode errors

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        return 1;
    }

    printf("[Log] Creating socket...\n");
    int enabled = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) == -1) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9990);

    printf("[Log] Binding socket to local address...\n");
    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }

    printf("[Log] Listening...\n");
    if (listen(server_sock, 10) == -1) {
        perror("listen");
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(server_sock, &master);
    int max_fd = server_sock;

    printf("Waiting for connections...\n");

    while (1) {
        fd_set reads;
        reads = master;

        if (select(max_fd + 1, &reads, NULL, NULL, NULL) == -1) {
            perror("select");
            return 1;
        }

        for (int i = 1; i <= max_fd; ++i) {
            if (FD_ISSET(i, &reads)) {
                if (i == server_sock) {
                    struct sockaddr_storage client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
                    if (client_sock == -1) {
                        perror("accept");
                        return 1;
                    }

                    FD_SET(client_sock, &master);
                    if (client_sock > max_fd) {
                        max_fd = client_sock;
                    }

                    char buf[256];
                    getnameinfo((struct sockaddr *)&client_addr, client_len, buf, 256, 0, 0, NI_NUMERICHOST);
                    printf("New connection from %s\n", buf);
                } else {
                    char buf[1024];
                    ssize_t bytes_read = recv(i, buf, 1024, 0);
                    if (bytes_read == -1) {
                        FD_CLR(i, &master);
                        close(i);
                        continue;
                    }

                    for (int j = 1; j <= max_fd; ++j) {
                        if (FD_ISSET(j, &master)) {
                            if (j == server_sock || j == i) {
                                continue;
                            }

                            (void)send(j, buf, bytes_read, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}