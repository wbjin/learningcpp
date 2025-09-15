/*
 * Compile with gcc getaddrinfo.c -o getaddrinfo
 * */
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * Does DNS lookup and filling out the structs to set your socket up.
 *
int
getaddrinfo(
    const char *hostname, // Name of service "https://google.com" or IP address
    const char *servname, // HTTP or port number. Check /etc/services for available services
    const struct addrinfo *hints,
    struct addrinfo **res
);
 *
 * You can use the hints addrinfo structs to specify what kind of socket you
 * want. There are a lot of elements int the addrinfo struct and you should read
 * the man page of getaddrinfo
 *
 * getaddrinfo will popultae a linked list pointed to by res.
 * */

int main() {
    {
        // To listen on your hosts's IP address on port 3490
        int status;
        struct addrinfo hints;
        struct addrinfo *res;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;
        // Setting the hostname as NULL in getaddrinfo and setting
        // this to AI_PASSIVe will set the socket address struct to
        // INADDR_ANY or IN6_ADDRANY_INIT
        hints.ai_flags = AI_PASSIVE;

        if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
            perror("getaddrinfo");
            exit(1);
        }

        freeaddrinfo(res);
    }

    {
        // To connect to a host
        int status;
        struct addrinfo hints;
        struct addrinfo *res;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;

        if ((status = getaddrinfo("www.google.com", "3490", &hints, &res)) != 0) {
            perror("getaddrinfo");
            exit(1);
        }

        // Print the IP address contained in the linked list structure
        for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
            if (p->ai_family == AF_INET) { // IPv4
                struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(addr->sin_family, &addr->sin_addr, ipstr, sizeof(ipstr));
                printf("IPv4: %s\n", ipstr);
            } else { // IPv6
                struct sockaddr_in6* addr = (struct sockaddr_in6*)p->ai_addr;
                char ipstr[INET6_ADDRSTRLEN];
                inet_ntop(addr->sin6_family, &addr->sin6_addr, ipstr, sizeof(ipstr));
                printf("IPv6: %s\n", ipstr);
            }
        }

        freeaddrinfo(res);
    }
}
