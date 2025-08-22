## Multiplexing and Demultiplexing

Pages 187-194 Section 3.2

The primary role of transport layer protocols is to expand *host to host*
communication enabled by the lower layers into *process to process*
communication. When a transport layer receives segments from the network layer,
it must be able to forward the segment to the appropriate process ie to the
intended socket. To be precise, the transport layer doesn't allow process to
process communication, it allows socket to socket communication meaning there
can be many independent communication channels in the same process. When a
transport layer receives segments, it *demultiplexes* it by inspecting the
source and destination port fields and forwards it to the correct socket. When
a transport layer sends segments, it attaches the source and destination port
headers. 

These port fields are each 16 bit wide meaning there can be 65535 ports. 0 to
1023 are well known port numbers reserved for well known protocols like ssh
(42) and http (80). When you create a socket, the socket is automatically
assigned a port. You can also assign a socket a specific port using `bind`.

### Connectionless muxing
Connectionless transport protocols like UDP have connectionless muxing. When
you send an UDP message, the message contains the source port and destination
port. UDP sockets are identified by a two tuple of the IP and the port. This
means that if two segments from different source or different ports send a
segment to the same IP and port tuple, the segments will arrive at the same
socket. The source port number in the header is required because the receiver
may want to send a message back and because UDP is connectionless, the source
of the segment is not known until the segment is received.

### Connection oriented muxing
TCP sockets are identified by a four tuple of source IP, source port,
destination IP, destination port. This means that segments from source IP and
port to the same destination IP and port will be forwarded to different
sockets. To understand why this is needed, consider how TCP connections are established
1. TCP server opens a "welcoming" socket and waits for connection establishment
requests.
2. TCP client creates a socket and sends a connection establishment request to
the "welcoming" socket. The connection establishment message is a simple TCP
segment that contains the source port of the client and a connection
establishment bit set in the header. 
3. The connection establishment segment is forwarded to the "welcoming" socket
and host accepts a connection. This creates a new socket and the rest of the
TCP handshake happens on this new socket. It is important to note that this new
socket has the same port as the "welcoming" socket. Note that when you send TCP
segments, you don't change the port per TCP connection. This is why TCP needs a
four tuple to identify sockets.
