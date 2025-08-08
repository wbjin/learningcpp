## Types of delay

Pages 35-43. Sections 1.4.1, 1.4.2

The types of delay are
- Nodal processing: Time it takes to inspect packet header and determine where
to direct. Typically implemented in hardware and recent switches/routers can do
this in < microsecond.
- Queueing delay: Time spent waiting at a node to be transmitted to next link.
Can be different depending on traffic at the node.
- Transmission delay: The time it takes to put all the bits of the packet onto
the wire. $\frac{L}{R}$
- Propagation delay: Time it takes a bit to propagate to next link. Typically
on the order of the speed of light so $\frac{distance}{speed}$ where speed is
typically $2 * 10^{8}$ to $3*10^{8} m/s$

NIC to NIC, without processing time, total delay can be considered as
$d_{nodal} = d_{process}+d_{queue}+d_{trans}+d_{prop}$

Qeueing delay:
Qeueing delay depends on the traffic at the node. Let $a$ be the rate at which
packets arive at a node in packets/sec, $R$ is the transmission rate or the
bandwidth of the wire, and $L$ is the average size of packets in bits. The
average rate at which bits arrive at the node is $La$ and the traffic intensity
would be $\frac{La}{R}$. A traffic intensity > 1 means traffic arrives faster
than it can be forwarded by the node and the queue will grow larger until it
starts to drop packets. If traffic intensity <= 1, the rate at which the queue
grows larger depends on the traffic where sparse packets won't experience
queueing delays but bursty workloads may.

For example, if $N$ packets arrive at the host simultaneously, the first packet
won't experience queueing delay. The subsequent packets will experiene queuing
delay of $(n-1)\frac{L}{R}$ where $n$ is the packets place in the queue.

End to end delay:
For two end systems that have $N$ links ($N-1$) routers, teh end to end delay
would be $N(d_{process}+d_{trans}+d_{prop})$ for a single packet that won't
experience queueing delay.
