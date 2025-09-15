/*
 * Compile with gcc connect.c -o connect
 * */
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*
 int
 connect(int socket, const struct sockaddr *address, socklen_t address_len);
 */

/*
 * connect allows you to establish a connection to the host if you have a TCP
 * socket. On a UDP socket, it sets the default destination. You can manually
 * populate the sockaddr struct or you can use the results of getaddrinfo
 * */

int main() {
    struct addrinfo* res;
    if (getaddrinfo("www.google.com", "http", NULL, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("socket");
        exit(1);
    }

    close(sockfd);
}
