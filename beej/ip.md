# IP

## Structs

`struct addrinfo` is used to prep the socket address structures for subsequent
use. It is also used for host and service name lookups.
```
struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
```

`ai_family` can be `AF_UNSPEC` to use either IPv4 or IPv6. This means you can
make your code IP version agnostic.

`ai_next` is a linke list of `addrinfo` struts and points to the next node in
list. There can be multiple nodes in a list if you get more than one match.

`ai_addr` is a pointer to `struct sockaddr` which is 
```
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
}; 
```
You usually don't need to write directly to these structs, instead use
`getaddrinfo()` but you do need to read it from at times.

In the `sockaddr` struct, `sa_family` is `AF_INET` or `AF_INET6` for internet
sockets. `sa_data` contains a destination address and port number for the
socket. This makes it hard to actually pack the data.

Instead of using `struct sockaddr`, there is a `struct sockaddr_in` made
specifically for internet sockets. The important thing is that `struct
sockaddr_in` can be cast to a pointer to a `struct sockaddr` and vice versa.
This is why even though `connect` takes a `struct sockaddr`, you can pass in a
`struct sockaddr_in` casted

`struct sockaddr_in` looks like
```
// (IPv4 only--see struct sockaddr_in6 for IPv6)

struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};
```
`sin_zero` is there just to pad the struct so it is the same size as `struct
sockaddr`. This struct makes it easier to specify the port and the address. The
struct can be interpreted as a `struct sockaddr` because the way the bytes are
laid out in memory are the same. The first two bytes is the `sa_family` in
`struct sockaddr` and `sin_family` (which should always be `AF_INET`) in
`struct sockaddr_in`. The port is the next 2 bytes in both structs, followed by
the address which in `struct sockaddr_in` is 
```
// (IPv4 only--see struct in6_addr for IPv6)

// Internet address (a structure for historical reasons)
struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
```
If you create a `struct sockaddr_in in_addr`, `in_addr.sin_addr.s_addr` is the
network byte order IPv4 address.

A similar struct for IPv6 exists
```
// (IPv6 only--see struct sockaddr_in and struct in_addr for IPv4)

struct sockaddr_in6 {
    u_int16_t       sin6_family;   // address family, AF_INET6
    u_int16_t       sin6_port;     // port, Network Byte Order
    u_int32_t       sin6_flowinfo; // IPv6 flow information
    struct in6_addr sin6_addr;     // IPv6 address
    u_int32_t       sin6_scope_id; // Scope ID
};

struct in6_addr {
    unsigned char   s6_addr[16];   // IPv6 address
};
```

`struct sockaddr_storage` exists to be large enough for both IPv4 and IPv6. In
cases where you don't know if `struct sockaddr` is going to be filled with IPv4
or IPv6, you can pass in this structure and cast it to the type you need.
```
struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

    // all this is padding, implementation specific, ignore it:
    char      __ss_pad1[_SS_PAD1SIZE];
    int64_t   __ss_align;
    char      __ss_pad2[_SS_PAD2SIZE];
};
```

## Utilities to deal with IP addresses
You can use the `inet_pton` function to convert IP addresses from
"10.12.110.57" or "2001:db8:63b3:1::3490" style to either `struct in_addr` or
`struct in6_addr` depending on what IP you specify. Usage looks something like
```
struct sockaddr_in sa;   // IPv4
struct sockaddr_in6 sa6; // IPv6

inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr));
inet_pton(AF_INET6, "2001:db8:63b3:1::3490", &(sa6.sin6_addr));
```

If you want to go the other way around, you can use `inet_ntop` to go from a
`struct in_addr`/`struct in6_addr` to a C-string.
```
char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string
struct sockaddr_in sa;      // pretend this is loaded with something
inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
printf("The IPv4 address is: %s\n", ip4);

char ip6[INET6_ADDRSTRLEN]; // space to hold the IPv6 string
struct sockaddr_in6 sa6;    // pretend this is loaded with something
inet_ntop(AF_INET6, &(sa6.sin6_addr), ip6, INET6_ADDRSTRLEN);
printf("The address is: %s\n", ip6);
```
