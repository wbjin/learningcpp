/*
 * Compile with gcc bind.c -o bind
 * */
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/*
 * bind is used to assign a port to a socket. This is usually done when you
 * listen on a specific port number for incoming connections.
 *
 int
 bind(int socket, const struct sockaddr *address, socklen_t address_len);
 *
 * Like socket, you can manually construct the arguments into bind. Usually, you
 * would use the results from getaddrinfo.
 *
 * If you don't close a socket properly, you can run into issues like "Address
 already in use". This means that you forgot to close a socket and the port is
 still bound to that socket. To allow sockets to reuse ports, you can use
 setsockopt
 * */

int main() {
    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints)); // Make sure to do this
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
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

    close(sockfd);
}
