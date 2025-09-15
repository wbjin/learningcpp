/*
 * Compile with gcc socket.c -o socket
 * */
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*
 * socket lets you control exactly what kind of socket you want. You can set
 * these options with hardcoded values if you now what protocol the other side
 * is running. However, you usually use the result of getaddrinfo for the socket
 * arguments.
 int
 socket(int domain, int type, int protocol);
 *
 * One thing about the domain is that when socket was created, the idea was that
 * you could ahve the IP address family (eg AF_INET) may support several
 * protocols (PF_INET). However, in reality, there is really one protocol for
 the IP address family so you can use AF_INET in places where PF_INET is used
 because they refer to the same thing. The correct thing to do however, would be
 to use AF_INET in struct sockaddr_in sin_family element and use PF_INET in
 */

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

    close(sockfd);
}
