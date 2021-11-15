#define _POSIX_C_SOURCE 200112L // for vscode errors

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
    printf("Configuraing local address...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    if (getaddrinfo(NULL, "8080", &hints, &bind_address) == -1) {
        perror("getaddrinfo");
        return 1;
    }

    printf("Creating socket...\n");
    int server_sock = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (server_sock == -1) {
        perror("socket");
        return 1;
    }

    printf("Binding socket to local address...");
    if (bind(server_sock, bind_address->ai_addr, bind_address->ai_addrlen) == -1) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(server_sock, 10) == -1) {
        perror("listen");
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_len);
    if (client_sock == -1) {
        perror("accept");
        return 1;
    }

    printf("Client is connected...\n");
    char buf[256];
    getnameinfo((struct sockaddr *)&client_address, client_len, buf, sizeof(buf), 0, 0, NI_NUMERICHOST);
    printf("\t%s\n", buf);

    char request[1024];
    ssize_t bytes_received = recv(client_sock, request, sizeof(request), 0);
    if (bytes_received == -1) {
        perror("recv");
        return 1;
    }
    printf("Received %zd bytes\n", bytes_received);

    printf("Sending response...\n");
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: text/plain\r\n\r\n"
                           "Hello World\n";

    ssize_t bytes_sent = send(client_sock, response, strlen(response), 0);
    if (bytes_sent == -1)  {
        perror("send");
        return 1;
    }
    printf("Sent %zd of %zd bytes\n", bytes_sent, strlen(response));

    printf("Closing connection...\n");
    close(client_sock);
    close(server_sock);

    printf("Finished\n");
    return 0;
}
