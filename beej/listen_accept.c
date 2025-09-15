/*
 * Compile with gcc listen_accept.c -o listen_accept
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
int
listen(int socket, int backlog);
 * */
/*
 * listen allows you to wait for incoming connections. You can specify how many
 * connections can be 'backlogged' in the queue while you are serving a
 * connection.
 * */

/*
int
accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
 * */
/*
 * accept allows you take a connection from the listen queue and create a new
 * socket fd to communicate with the connection. This new socket file descriptor
 * can be used to send and receive messages. You can see where the connection
 * came from using the sockaddr_storage struct and passing it into the accept
 * call. accept also takes in a pointer to the size of this struct. It will
 * overwrite the sockaddr_storage struct and set the size to actual size that is
 * used to represent the other side. Note that calling accept will block until
 * there is a connection to accept
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

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    int connfd;
    struct sockaddr_storage addr;
    socklen_t size = sizeof(addr);
    if ((connfd = accept(sockfd, (struct sockaddr*)&addr, &size)) == -1) {
        perror("accept");
        close(sockfd);
        exit(1);
    }

    close(connfd);
    close(sockfd);
}
