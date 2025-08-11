## Socket Programming

Pages 152- Section 2.7

The `socket` functions creates an endpoint for a process to interact with the
host's networking stack. It returns a file descriptor
```
 int
 socket(int domain, int type, int protocol);
```
- domain: A communication donmain such as AF_INET (internet communication) or
AF_UNIX (local Unix Domain Socket communication). Note, in the man pages socket
domain flags have the prefix PF_. In Linux, PF_ flags are essentially aliases
to the AF_ counterparts. Originally, the intentnion was that a single protocol
family may multiple different addressing scheme, ie internet would have more
than just IP addresses for addressing but this didn't really happen.
- type: SOCK_STREAM, SOCK_DGRAM, or SOCK_RAW (only available to super users)
- protocol: Usually, there is only one protocol per socket type SOCK_STREAM =
TCP, SOCK_DGRAM = UDP but you can have different protocols for different socket
types. 

UDP Socket Programming:

With UDP sockets, the recipient's address and port can be specified per
message. This means that it is passed into `sendto` and the receiver can
inspect where the datagram came from from `recvfrom`. As the server, you have
to `bind` to a specific address and port. This means that any messages intended
for the hostname and port will be sent through the socket.
```
Server                                  Client
socket(AF_INET, SOCK_DGRAM)             socket(AF_INET, SOCK_DGRAM)
bind
recvfrom                <-----------    sendto
sendto                  ----------->    recvfrom
close                                   close
```

TCP Socket Programming:

TCP is special in that it is a connection based transport. The server needs a
socket that listens for new connections. A client initiates the "three way
handshake" with the server's listening socket. When a client is trying to
initiate a connection with the server, a **new** socket is created. From the
application, the socket's will act as pipes. Bytes will be delivered, reliably,
in order, as a stream and the server and client can communicate through this
pipe. Client's have to `connect` to a server before sending and receiving
messages. This is initiates the three way handshake and establishes a
connection.
```
Server                                  Client
socket(AF_INET, SOCK_STREAM)            socket(AF_INET, SOCK_STREAM)
bind                                    
listen                                  
accept                  <---------->    connect
recv                    <-----------    send
send                    ----------->    recv
close                                   close
```
