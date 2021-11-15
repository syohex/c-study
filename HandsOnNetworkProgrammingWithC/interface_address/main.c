#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) == -1) {
        perror("getifaddrs");
        return 1;
    }

    struct ifaddrs *p = addresses;
    while (p != NULL) {
        if (p->ifa_addr == NULL) {
            p = p->ifa_next;
            continue;
        }

        int family = p->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            bool is_v4 = family == AF_INET;

            printf("name: %s\t", p->ifa_name);
            printf("family: %s\t", is_v4 ? "IPv4" : "IPv6");

            char buf[128];
            if (is_v4) {
                getnameinfo(p->ifa_addr, sizeof(struct sockaddr_in), buf, sizeof(buf), 0, 0, NI_NUMERICHOST);
            } else {
                getnameinfo(p->ifa_addr, sizeof(struct sockaddr_in6), buf, sizeof(buf), 0, 0, NI_NUMERICHOST);
            }

            printf("\t%s\n", buf);
        }

        p = p->ifa_next;
    }

    return 0;
}