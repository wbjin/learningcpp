/*
 * Compile with gcc poll.c -o poll
 * */
#include <arpa/inet.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

/*
int
poll(struct pollfd fds[], nfds_t nfds, int timeout);

struct pollfd {
    int    fd;        
    short  events;   
    short  revents;  
};
 **/
/*
 * poll does the polling of sockets for us. You give it an array of struct
 * pollfd when there is data in one of the sockets, the poll function will
 * return.
 *
 * fd is the file descriptor we want to poll and events contains the bitwise-OR
 * of various flags. Some flags are POLLIN which alerts when data is ready to
 * recv on the socket, POLLOUT which alers when the socket is ready to send
 * dasta without blocking, and POLLHUP when a remote connection closes the
 * connection. There are more flags so check the man page.
 *
 * You pass in the array of pollfd, the size, and a timeout in milliseconds
 * which can be negative to wait forever. The return value of poll will be the
 * number of file descriptors that were alerted, -1 on an error, and 0 if a
 * timeout happens. The value of revents can be checked to see if an event
 * occured.
 * */

int main() {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo* res;
    if (getaddrinfo(NULL, "8080", &hints, &res) == -1) {
        perror("getaddrinfo");
        exit(1);
    }

    int listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(listenfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    // Limit number of sockets to 10
    struct pollfd pfds[10];
    pfds[0].fd = listenfd;
    pfds[0].events = POLLIN;
    int pfd_count = 1;

    while (1) {
        int count = poll(pfds, pfd_count, -1);
        if (count == -1) {
            perror("poll");
            continue;
        }

        for (int i = 0; i < pfd_count; ++i) {
            if (pfds[i].revents & (POLLIN | POLLHUP)) {
                if (pfds[i].fd == listenfd) {
                    struct sockaddr_storage addr;
                    socklen_t size = sizeof(addr);
                    int connfd = accept(listenfd, (struct sockaddr*)&addr, &size);
                    if (addr.ss_family == AF_INET) {
                        struct sockaddr_in* in_addr = (struct sockaddr_in*)&addr;
                        char addr_str[INET_ADDRSTRLEN];
                        inet_ntop(in_addr->sin_family, &addr, addr_str, sizeof(addr_str));
                        printf("new connection from IPv4 %s on fd %d\n", addr_str, connfd);
                    } else {
                        struct sockaddr_in6* in6_addr = (struct sockaddr_in6*)&addr;
                        char addr_str[INET6_ADDRSTRLEN];
                        inet_ntop(in6_addr->sin6_family, &addr, addr_str, sizeof(addr_str));
                        printf("new connection from IPv6 %s on fd %d\n", addr_str, connfd);
                    }
                    if (pfd_count == sizeof(pfds)) {
                        printf("not accepting any more connections\n");
                        close(connfd);
                    } else {
                        pfds[pfd_count].fd = connfd;
                        pfds[pfd_count].events = POLLIN;
                        ++pfd_count;
                    }
                } else {
                    if (pfds[i].revents & POLLIN) {
                        char msg[1024];
                        int rcvd;
                        if ((rcvd = recv(pfds[i].fd, msg, sizeof(msg), 0)) == -1) {
                            perror("recv");
                            continue;
                        }
                        if (rcvd == 0) {
                            printf("%d closing\n", pfds[i].fd);
                            close(pfds[i].fd);
                            pfds[i] = pfds[pfd_count-1];
                            --pfd_count;
                        } else {
                            msg[rcvd] = '\0';
                            printf("client message: %s\n", msg);

                            if (send(pfds[i].fd, msg, rcvd, 0) == -1) {
                                perror("send");
                                continue;
                            }
                        }
                    } else if (pfds[i].revents & POLLHUP) {
                        printf("%d closing\n", pfds[i].fd);
                        close(pfds[i].fd);
                        pfds[i] = pfds[pfd_count-1];
                        --pfd_count;
                    }
                }
            }
        }
    }

    close(listenfd);
}
