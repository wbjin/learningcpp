/*
 * Compile with gcc send_recv.c -o send_recv
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
ssize_t
send(int socket, const void *buffer, size_t length, int flags);
 * */
/*
 * send is how you send messages over stream sockets. send may not send all of
 * the data you want it to. It is up to the application to resend the message in
 * this case. If the size of the message is less than 1Kb, it will probably
 * send. send can also block on rare ocassions
 * */
/*
ssize_t
recv(int socket, void *buffer, size_t length, int flags);
 * */
/*
 * recv is a blocking call that allows you to put the message sent from the
 * other host into a buffer you allocate. recv is a blocking call meaning it
 * will block until there is some data ready to receive.
 * */

/*
 * sockets are blocking by default. You can change a socket to nonblocking using
 * the fcntl function. A non blocking socket will return -1 and errno will be
 * set to either EAGAIN or EWOULDBLOCK. When recv returns 0, it means the other
 * side has closed the connection.
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

    char buf[1024];
    ssize_t bytes = 0;
    ssize_t recvd = 0;
    do {
        recvd = recv(connfd, buf+bytes, sizeof(buf)-bytes, 0);
        if (recvd == -1) {
            perror("recv");
            break;
        }
        bytes += recvd;
    } while (bytes < sizeof(buf) && recvd != 0);

    bytes = 0;
    do {
        ssize_t sent = send(connfd, buf+bytes, sizeof(buf)-bytes, 0);
        if (sent == -1) {
            perror("sent");
            break;
        }
        bytes += sent;
    } while (bytes < sizeof(buf));

    close(connfd);
    close(sockfd);
}
