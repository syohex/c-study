#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

static const int NUM_CHILDREN = 10;
static const int PORT = 50003;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&enable, sizeof(int)) == -1) {
        perror("setsockopt(SO_REUSEADDR)");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        return -1;
    }

    if (listen(sock, 10) == -1) {
        perror("listen");
        return -1;
    }

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid != 0) {
            // parent
            continue;
        }

        while (1) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof client_addr;
            int client_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_len);
            if (client_sock == -1) {
                perror("accept");
                return -1;
            }

            char buffer[256];
            while (1) {
                errno = 0;
                ssize_t n = read(client_sock, buffer, 256);
                if (n == -1) {
                    if (errno == EINTR) {
                        continue;
                    }

                    perror("read");
                    return -1;
                }
                if (n == 0) {
                    break;
                }

                for (ssize_t i = 0; i < n; ++i) {
                    buffer[i] = toupper(buffer[i]);
                }

                if (send(client_sock, buffer, n, 0) == -1) {
                    perror("send");
                    return -1;
                }
            }

            close(client_sock);
        }
    }

    while (waitpid(-1, NULL, 0) > 0)
        ;

    close(sock);
    return 0;
}