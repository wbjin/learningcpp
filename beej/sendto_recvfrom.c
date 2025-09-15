/*
 * Compile with gcc sendto_recvfrom.c -o sendto_recvfrom
 * */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*
ssize_t
sendto(int socket, const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
 * */
/*
 * Because datagram sockets aren't connected with a peer, the sendto function
 * needs to take in the destination address to know who to send it to.
 * */
/*
ssize_t
recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict address, socklen_t *restrict address_len);
 * */
/*
 * recvfrom also takes in a struct sockaddr so the application can know who the
 * messsage was sent from.
 * */

int main() {
    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); // Make sure to do this
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, "3490", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    char buf[1024];
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);
    ssize_t rcvd;
    if ((rcvd = recvfrom(sockfd, buf, sizeof(buf), 0,
                            (struct sockaddr *)&peer_addr, &peer_addr_size)) == -1) {
        perror("recvfrom");
        close(sockfd);
        exit(1);
    }

    if (peer_addr.ss_family == AF_INET) {
        struct sockaddr_in* addr = (struct sockaddr_in*)&peer_addr;
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(peer_addr.ss_family, &addr->sin_addr, addr_str, sizeof(addr_str));
        printf("from IPv4 address %s\n", addr_str);
    } else {
        struct sockaddr_in6* addr = (struct sockaddr_in6*)&peer_addr;
        char addr_str[INET6_ADDRSTRLEN];
        inet_ntop(peer_addr.ss_family, &addr->sin6_addr, addr_str, sizeof(addr_str));
        printf("from IPv6 address %s\n", addr_str);
    }

    if (sendto(sockfd, buf, rcvd, 0, (struct sockaddr*)&peer_addr, peer_addr.ss_len) == -1) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    close(sockfd);
}
