# Multicast

Multicast is a way to send data from one sender to a group of receivers at
once. Receivers join a multicast group, and the network distributes the data to
them efficiently. Unlike unicast (one-to-one) or broadcast (to everyone on a
network), multicast targets only the group. It’s commonly used for live video
streams and distributing real-time data.

## Multicast Addresses
CIDR prefixes with 224.0.0.0/4 (the first four bits) belong to multicast
addresses. This is also historically class D in the classful IP address
paradigm. IP packets with destination address within 224.0.0.0/4 (those that
begin with 1110) is an IP multicast datagram. The remaining 28 bits identify
the group. There are special reserved addresses that mean a specific thing.
While the valid multicast address space is 224.0.0.0 - 239.255.255.255, the
following should not be used:
- 224.0.0.1: All hosts group. A way to ping all mutlicast capable hosts on the
network
- 224.0.0.2: All routers group. All multicast routers must join that group on
all its interfaces.
- 224.0.0.4, 224.0.0.5, 224.0.0.13: DVMRP, OSPF, PIM routers
- 224.0.0.0 through 224.0.0.255: Loacl scoping and should not leave the router.
- 239.0.0.0 to 239.255.255.255: Admin scoping

Multicast can also be source specific if in the 232.0.0.0/8 address group. The
receives can specify a source and a group address to subscribe to.

IPv6 multicast uses ff00::/8 for as its subnet and uses MLD for membership
reporting.

## Conformance
Hosts can have various levels of conformity to mulicast.
- Level 0: No support for multicast. Can't send or receive and must ignore all
multicast datagrams.
- Level 1: Support for sending but not receiving. Cannot receive or join
multicast groups.
- Level 2: Send, receive, and join multicast groups. Implements IGMP (Internet
Group Management Protocol).

## Sending Multicast Datagrams
Multicast datagrams can be sent as UDP datagrams with the destination IP
address within 224.0.0.0./4. Userspace can determine several fields.

The TTL can be set to:
- 0: For local host, within the machine only.
- 1: Within the LAN, routers cannot forward
- 2-255: Allows progressively more router hops

Loopback can be determined where a local copy of the multicast datagram can be
sent to the loopback interfaces on the machine. This allows other processes to
see the datagram. This can be done with `IP_MULTICAST_LOOP`. In POSIX compliant
systems, the datagram is placed in the sending socket.

The interface to send the datagram out of can also be chosen. `IP_MULTICAST_IF`
selects the multicast interface. Otherwise, the system chooses.

These options are set with `setsockopt` at the `IPPROT_IP` layer.

## Receiving Multicast Datagrams
To receive multicast datagrams, you first have to join a multicast group. The
application must fist bind a socket to a port, use `IP_ADD_MEMBERSHIP` to join
a multicast group (and also specify an interface), and then start receiving
multicast packets.

Membership to a multicast group are at two levels, at the socket and and at the
interface. At the socket level, it pertains to whether or not the application
is still interested in that multicast group. At the interface level, it
pertains to whether or not that host is interested in the multicast group. If
two sockets join the same group on the same interface, one of the sockets
leaving does not remove the other's membership. The host remains a member on
that interface and only after all sockets leave, then will the interface leave
the group. In Linux, it is possible to see multicast datagrams from a group
that you left if other sockets on the system joined the same group on the same
interface. This is because `IP_MULTICAST_ALL` is enabled by default.

## Kernel Configuration
Linux configuration for multicast depends on whether it is for an end host or
router. You generally need:
- `CONFIG_IP_MULTICAST=y`
- An interface that is up that supports multicast and has the appropriate IP
address
- Permit multicast traffic on your firewall

You can check your interfaces with `ip link show`

Multicast forwarding state can be inspected with `ip mroute show`.

`cat /proc/net/igmp` shows IPv4 multicast membership information for the
current network namespace.

## Multicast Programming
Most of the multicast specific programming is done through `setsockopt` and
`getsockopt`.

`IP_MULTICAST_LOOP`: Enable or disable other sockets on the host subscribed to
the mulitcast group receiving the datagram.
```C
int loop = 0;  // Disable local multicast loopback.
if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) == -1) {
    perror("IP_MULTICAST_LOOP");
}

int current_loop = 0;
socklen_t length = sizeof(current_loop);
if (getsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &current_loop, &length) == -1) {
    perror("getsockopt");
}
```

`IP_MULTICAST_TTL`: Default value of 1.
```C
int ttl = 1;
if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) == -1) {
    perror("IP_MULTICAST_TTL");
}
```

`IP_MULTICAST_IF`: Interface to bind multicast traffic to
```C
in_addr interface_addr{};

// IPv4 address assigned to multicast capable interface.
if (inet_pton(AF_INET, "192.168.1.20", &interface_addr) != 1) {
    std::fprintf(stderr, "Invalid IPv4 address\n");
} else if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &interface_addr,
                      sizeof(interface_addr)) == -1) {
    std::perror("IP_MULTICAST_IF");
}
```

`IP_ADD_MEMBERSHIP`: Join a multicast group.
`IP_DROP_MEMBERSHIP`: Leave a multicast group.
```C
ip_mreq membership{};

// The multicast group to receive.
if (inet_pton(AF_INET, "239.192.0.10",
              &membership.imr_multiaddr) != 1) {
    std::fprintf(stderr, "Invalid IPv4 address text\n");
    return 1;
}

// Can also be INADDR_ANY if you want kernel to choose
// Your receiving interface's LOCAL IPv4 address.
// Replace this with the address assigned to ens160/eth0.
if (inet_pton(AF_INET, "192.168.1.20",
              &membership.imr_interface) != 1) {
    std::fprintf(stderr, "Invalid interface address\n");
    return 1;
}

// Join the group on that interface.
if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
               &membership, sizeof(membership)) == -1) {
    std::perror("IP_ADD_MEMBERSHIP");
    return 1;
}

// Receive packets using recvfrom() here...

// Later: leave the same group on the same interface.
if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
               &membership, sizeof(membership)) == -1) {
    std::perror("IP_DROP_MEMBERSHIP");
    return 1;
}
```

## IGMP
Internet Group Management Protocol is the protocol for joining and leaving a
multicast group. A socket joins or leaves with `IP_ADD/DROP_MEMBERSHIP`. The
kernel aggregates this membership information into per interface states and
sends IGMP reports according to this shared state. The host will tell its
network's router that it is interested in a specific multicast group. The
router will in turn tell its upstream router that it is interested in a
multicast group, and so on. Therefore, a multicast group registration from a
single socket can affect the interface on that host and the other hosts on the
network, and so on.

Switches also participate in IGMP traffic. It can snoop IGMP packets to
determine which specific ports to send multicast traffic to without having to
flood all of its ports.

IGMPv3 has two main formats, membership query and membership report. It is
carried inside IPv4.

Membership query: The TTL in the IP packet is set to 1. The size of the packet
is 12 + 4N where N is the number of unicast addresses of multicast senders  you
are querying. A multicast router sends a Membership Query to discover whether
hosts on its local network still want multicast traffic for all groups, a
specific group, or a specific group from particular sources.

- Type: 0x11 for query
- Max Resp Code: Response deadline. Randomize the time to respond back by,
bounded by this number to spread traffic out
- Checksum: Detect message corruption
- Group address: The group that is being queried
- N: Number of source addresses in the next part
- Source Address: Unicast address of the mulicast sender whose traffic is being
queried

```txt
 0               8              16                             31
+---------------+---------------+-------------------------------+
| Type = 0x11   | Max Resp Code |           Checksum            |
+---------------+---------------+-------------------------------+
|                         Group Address                         |
+-------+-+-----+---------------+-------------------------------+
| Flags |S| QRV |     QQIC      |     Number of Sources (N)     |
+-------+-+-----+---------------+-------------------------------+
|                       Source Address 1                        |
+---------------------------------------------------------------+
|                              ...                              |
+---------------------------------------------------------------+
|                       Source Address N                        |
+---------------------------------------------------------------+
```

Membership report: A host sends a Membership Report to inform local multicast
routers which multicast groups and sources it wants to receive, or how those
interests have changed.

- Type: 0x22
- Checksum: Detect errors
- Flags
- Number of group records
- Group records: Variable siez descriptions of registration or change in a
multicast group

- Record type: Determines how to interpret source list.
    - `MODE_IS_INCLUDE`: Receive only these sources
    - `MODE_IS_EXCLUDE`: Receive all sources but these
    - `CHANGE_TO_INCLUDE_MODE`: Switch to include using the list
    - `CHANGE_TO_EXCLUDE_MODE`: Switch to exclude using the list
    - `ALLOW_NEW_SOURCES`: New sources are wanted
    - `BLOCK_OLD_SOURCES`: These sources are no longer wanted

- Auxillary data length: Length in 32 bit words, enables parser to locate the
next group record
- Number of sources
- Multicast address: Group whose reception state is being described
- Source Addresses: Senders to include, exclude, allow, or block
- Aux Data: Extensions
```txt
 0               8              16                             31
+---------------+---------------+-------------------------------+
| Type = 0x22   |   Reserved    |           Checksum            |
+---------------+---------------+-------------------------------+
|             Flags             | Number of Group Records (M) |
+-------------------------------+-------------------------------+
|                  Group Record 1 — variable length             |
+---------------------------------------------------------------+
|                              ...                              |
+---------------------------------------------------------------+
|                  Group Record M — variable length             |
+---------------------------------------------------------------+

Each group record:
+---------------+---------------+-------------------------------+
|  Record Type  | Aux Data Len  |     Number of Sources (N)     |
+---------------+---------------+-------------------------------+
|                       Multicast Address                       |
+---------------------------------------------------------------+
|                       Source Address 1                        |
+---------------------------------------------------------------+
|                              ...                              |
+---------------------------------------------------------------+
|                       Source Address N                        |
+---------------------------------------------------------------+
|                 Auxiliary Data — if present                   |
+---------------------------------------------------------------+
```
