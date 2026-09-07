# TCP

### Keepalive
TCP keepalive is an optional TCP mechanism for detecting if the other side is
still reachable. After a threshold of inactivity, the kernel will send a probe
trying to reach the peer. This probe is an ACK packet with 0 payload data. If
several probes are unanswered, the connection is torn down and an error is
reported to userspace. In Linux, this can be configured with the following
socket options:
- `SO_KEEPALIVE`: Enable TCP keepalive for a socket
- `TCP_KEEPIDLE`: Seconds of idle time before first probe is sent
- `TCP_KEEPINTVL`: Seconds before unanswered probes
- `TCP_KEEPCNT`: Number of unanswered probes before shutdown

### Termination
TCP termination can happen gracefully or abruptly. In the graceful scenario,
the "active" side (side that initiated close) will send a FIN and wait for an
ACK. Then the "passive" side will also then send a FIN and wait for an ACK.
```txt
Active closer                         Passive closer
─────────────                         ──────────────
close()
FIN  ───────────────────────────────►
     ◄────────────────────────────── ACK
                                      close()
     ◄────────────────────────────── FIN
ACK  ───────────────────────────────►
```
In the abortive case, a RST packet may be sent. This means to abort the
connection immediately, discarding any buffered packets. You may receive this
if the application on the other side has closed the socket or terminated, a
firewall rejects the packet. If the kernel has detected a timeout, whether due
to a series of unACKed packets or missed keepalive probes, it will close the
socket without sending any FINs.

### Selective ACKs
Selective ACKs is a TCP option that allows TCP to ACK each packet individual
(opposed to cumulative ACKs which is the default). This is communicated during
the three way handshake where the SYN or SYN-ACK packets will have a set SACK
option. With TCP, the cumulative ACK is still present. However, if a peer
receives out of order data, it will add a SACK block saying which data range it
has already received.

### WSCALE
TCP WSCALE (window scale) is a way to advertise more than the default receiver
window which is 65535 bytes (2^16). The WSCALE option allows you to specify a
scale value that determines how much the 16 bit window is left shifted by (16
bit window field << scale). This allows you to scale the window up to 1GB. This
can be specified with `net.ipv4.tcp_window_scaling` through sysctl. This allows
the scale factor to be 0 through 14, allowing a max of 65535 << 14 which is
around 1GB.

### Nagles
Nagle's algorithm is a TCP sender side optimization that batches smaller
packets together if there are unACKed packets in flight. It can be set using
`TCP_NODELAY` option on a socket where setting the value to 1 indicates not to
use Nagle's and setting it 0 tells the kernel to restore Nagle behavior..
`TCP_CORK` is a similar sender side optimziation that holds onto packets until
the application requests more data to be sent.

### Delayed ACKs
Delayed ACKs is a receiver side option that waits to send out ACKs to try to
get more data to send the ACK with. There isn't a reliable way to set this but
`TCP_QUICKACK` tells the kernel to temporary immediately ACK every packet.

### Urgent pointer
Urgent pointer is a 16 bit field in the header that marks a position in the
byte stream as urgent data that is used to tell the application to process
something promptly. You can send urgen data with `send(..., MSG_OOB)` and
receive urgent data with `recv(..., MSG_OOB)`. If `SO_OOBINLINE` on a socket is
enabled, it will be part of the regular bytestream. The kernel will notify
userspace with `SIGURG`, `POLLPRI`, or `select`'s exception set. It will
prevent regular receives from crossing the urgent pointer until it is read with
`recv(..., MSG_OOB)`.
