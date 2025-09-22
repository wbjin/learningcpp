/*
 * Compile with gcc select.c -o select
 * */
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

/*
int select(int numfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout); 

FD_SET(int fd, fd_set *set): Add fd to the set
FD_CLR(int fd, fd_set *set): Remove fd from the set
FD_ISSET(int fd, fd_set *set): Return true if fd is in the set
FD_ZERO(fd_set *set): Clear all entries in the set

struct timeval {
    int tv_sec;     // seconds
    int tv_usec;    // microseconds
}; 
 **/

/*
 * select is a way to monitor several sockets at the same time. It tells you
 * which socket is ready for reading and which for writing. select monitors sets
 * of file descriptors. numfds is the value of the highest file descriptor
 * plus 1. When select returns, readfds will be modified to reflect which of the
 * file descriptors is ready for reading. You can check if a file descriptor is
 * ready to read with the FD_ISSET macro.
 *
 * Sets are manipulated with preprocessor macros. You also pass in a timeval
 * struct to specify how long select should wait before returning. Setting the
 * fields in the timeval struct to 0 will mean select wil timeout immediately
 * after inspecting all of the fds. Passing in NULL will mean it will never
 * timeout.
 *
 * When the other side closes its side of the socket, select will put that
 * socket in the read set and recv on it will return 0
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

    fd_set master; // make a set to keep track of all of our fds
    // std::array<int, 100> master; // optionally, we can keep an array of fds
    // and copy it into readfds before every select call
    FD_ZERO(&master); // clear master set
    FD_SET(listenfd, &master); // add listen socket to master set

    fd_set readfds; // make a set for the fds that are ready to be read from
    FD_ZERO(&readfds); 

    int max = listenfd; // we need to keep track of the highest fd value

    while (1) {
        FD_COPY(&master, &readfds); // select will change this set with fds that
                                    // are ready to be read from
        // readfds = master; // if FD_COPY not available

        // can also use select for keeping track of fds that are ready to be
        // written or have exceptions but for this just reads
        // no timeout
        if (select(max+1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i < max + 1; ++i) {
            if (FD_ISSET(i, &readfds)) {
                // this fd is ready to be read from
                if (i == listenfd) {
                    // new connection request
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

                    // add connfd to set and make sure to update max
                    FD_SET(connfd, &master);
                    if (connfd > max)
                        max = connfd;
                } else {
                    // data ready to be read
                    char msg[1024];
                    int rcvd;
                    if ((rcvd = recv(i, msg, sizeof(msg), 0)) == -1) {
                        perror("recv");
                        continue;
                    }
                    if (rcvd == 0) {
                        printf("%d closing\n", i);
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        msg[rcvd] = '\0';
                        printf("client message: %s\n", msg);

                        if (send(i, msg, rcvd, 0) == -1) {
                            perror("send");
                            continue;
                        }
                    }
                }
            }
        }
    }
}
