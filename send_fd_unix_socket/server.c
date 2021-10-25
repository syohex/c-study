#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "common.h"

static int do_sendmsg(int sock, int fd) {
    char buf[CMSG_SPACE(sizeof(int))];

    char ch = '!';
    struct iovec iov[1];
    iov[0].iov_base = &ch;
    iov[0].iov_len = sizeof(ch);

    struct cmsghdr *chdr = (struct cmsghdr *)buf;
    chdr->cmsg_len = CMSG_LEN(sizeof(int));
    chdr->cmsg_level = SOL_SOCKET;
    chdr->cmsg_type = SCM_RIGHTS;

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = chdr;
    msg.msg_controllen = CMSG_LEN(sizeof(int));

    int *data = (int *)CMSG_DATA(chdr);
    *data = fd;

    if (sendmsg(sock, &msg, 0) == -1) {
        perror("sendmsg");
        return -1;
    }

    return 0;
}

int main(void) {
    int sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, TEST_SOCK_PATH);

    socklen_t addr_len = sizeof(addr);
    if (bind(sock, (struct sockaddr *)&addr, addr_len) == -1) {
        perror("bind");
        return 1;
    }
    if (listen(sock, 5) == -1) {
        perror("listen");
        return 1;
    }

    while (1) {
        int client = accept(sock, NULL, 0);
        if (client == -1) {
            perror("accept");
            return 1;
        }

        int fd = open("test.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
        if (fd == -1) {
            perror("open(test.txt)");
            return 1;
        }

        printf("## send fd=%d\n", fd);
        if (do_sendmsg(client, fd) != 0) {
            return 1;
        }
    }

    return 0;
}