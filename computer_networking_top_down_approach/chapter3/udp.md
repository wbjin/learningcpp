## UDP

Pages 195-200 Section 3.3

UDP is a connection less transport protocol that handles muxing and error
checking. It provides very little in addition to IP to allow maximum
flexibility for users. UDP provides the following benefits over TCP
- With UDP, when you pass data into a socket to send, it is sent packaged with
the UDP header and passed immediately to the network layer. With TCP, there are
factors such as congestion control that can delay when a segment is sent. This
makes it undesirable for usages where you need to maintain a certain throughput
at all times.
- You don't need to initiate a connection to start sending messages. TCP
requires a three-way handshake which is 1.5 RTT just to start sending segments. 
- UDP doesn't require connection state. With TCP, each connection needs to
maintain state such as the receive and send buffers, congestion control
parameters, sequence and acknoqledgements, etc
- UDP has much smaller packet header overhead with 8 bytes instead of TCP's 20
bytes.

### UDP segment structure
A UDP segment consists of four fields, each with two bytes. The source port,
destination port, length of the UDp segment in bytes (header plus data), and
checksum.
```
source port # 2 bytes | destination port # 2 bytes
length 2 bytes        | checksum 2 bytes
application data
```

The checksum is used to determine whether bits in the segment have been
altered. The checksum is calculated by performing 1s complement of the sum of
all 16 bit word segments where overflows wrap around. For example, if we have
the following three 16 bit words
```
0110011001100000
0101010101010101
1000111100001100
```
The sum of the first two of these 16 bit words is
```
0110011001100000
0101010101010101
________________
1011101110110101
```
The sum of all thre 16 bit words is
```
1011101110110101
1000111100001100
_________________
0100101011000010
```
The 1s complement is obtained by flipping all the bits. The checksum for this
segment would be
```
1011010100111101
```
The receiver then does the same operation including the checksum. If no bits
were changed, the sum should add up to all 1s. UDP needs to provide error
checking at the protol layer because there is no guarantee that link layer
implementaions have error checking. There could also be bit errors when a
segment is stored in an intermediate node's memory. UDP doesn't do anything to
recover from an error. Some implementaions discard the segment while others may
pass the datamaged segment to the application with a warning.
