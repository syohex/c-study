#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"

static int do_recvmsg(int sock) {
    char ch = '*';
    struct iovec iov[1];
    char buf[CMSG_SPACE(sizeof(int))];

    iov[0].iov_base = &ch;
    iov[0].iov_len = sizeof(ch);

    struct cmsghdr *ctrl_hdr = (struct cmsghdr *)buf;
    ctrl_hdr->cmsg_len = CMSG_LEN(sizeof(int));
    ctrl_hdr->cmsg_level = SOL_SOCKET;
    ctrl_hdr->cmsg_type = SCM_RIGHTS;

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = ctrl_hdr;
    msg.msg_controllen = CMSG_LEN(sizeof(int));
    msg.msg_flags = 0;

    if (recvmsg(sock, &msg, 0) == -1) {
        perror("recvmsg");
        return -1;
    }

    int fd = *(int *)CMSG_DATA(ctrl_hdr);
    printf("## Got fd=%d\n", fd);
    write(fd, "hello world\n", 12);
    close(fd);
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

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        return 1;
    }

    if (do_recvmsg(sock) != 0) {
        return 1;
    }

    return 0;
}