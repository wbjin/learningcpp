#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

void server() {
    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, "12345", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) ||
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes))) {
        perror("setsockopt");
        close(sockfd);
        exit(1);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen)) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    char buf[1024];
    ssize_t rcvd = 0;
    if ((rcvd = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL)) == -1 ) {
        perror("recvfrom");
        exit(1);
    }

    fwrite(buf, sizeof(char), rcvd, stdout);

    freeaddrinfo(res);
    close(sockfd);
}

void client() {
    struct addrinfo* res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo("128.105.145.119", "12345", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(1);
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    char msg[] = "hello from client";
    if (sendto(sockfd, msg, sizeof(msg), 0, res->ai_addr, res->ai_addrlen) == -1) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    freeaddrinfo(res);
    close(sockfd);
}

int main(int argc, char** argv) {
    sleep(3);
    if (strncmp(argv[1], "server", sizeof("server")) == 0) {
        server();
        return 0;
    }

    client();
}
