

This is the analysis of the function graph generated on the transmit path. This
was done on 6.8.0.

`__sys__sendto`: Entrypoint into the kernel

`sockfd_lookup_light`: Validate the file descriptor, check that it is a BSD
socket, return the `socket` structure associated with fd.

`move_addr_to_kernel`: Copy over the user space `sockaddr_storage` structure
into kernel space. This structure contains the destination address. The kernel
copies it over to kernel space so that it doesn't have to keep on dereferencing
userspace pointers.

`inet_sendmsg`: This is where the dispatch to the corresponding protocol send
function happens. `socket` is a generic socket layer object while `sock` is a
protocol specific socket object that contains pointers to things like the read
and write queues, protocol state, ports, addresses, etc. `sock.sk_prot` is the
transport protocol specific operations table that contains functions for
sending and receiving, etc. For a `SOCK_DGRAM` socket, it dispatches to
`udp_sendmsg`

`udp_sendmsg`: The main implementatino of a send with `UDP` socketsoO

`ip_route_output_flow`: IPv4 routing lookup function used for outgoing packets
do determine the next hop, which interface to send it out of, etc. It takes in
as input a `flowi4` struct that describes the packet flow with fields like
`saddr`, `daddr`, `flowi4_oif`, `flowi4_iif` (output and input interface),
`flowi4_proto`, `flowi4_sport`, `flowi4_dport`. It returns a `rtable *` which
is a IPv4 route cache entry specifiying the output network device and next hop
router. Won't go into the entire router lookup process because it's
complicated. 

`ip_make_skb`: Constructs the outgoing packet `sk_buff` containing the IP
header. It builds the packet data structure containing metadata and actual
packet data as a `sk_buff` struct. This only alloactes the `sk_buff` and
doesn't actually put data into it yet. At this point, the payload is in the
data buffer but headers are not populated.

Aside: `sk_buff` (socket buffer). `sk_buff` is the central packet object that
gets passed through the network stack. It contains data to packet metadata and
pointers to the actual packet bytes. In memory, the metadata comes first in the
struct and then the pointers to packet data comes after with the `head`,
`data`, `tail`, `end` pointers along with the size. In memory, there is
headroom, then actual packet data, then tailroom. This allows prepending of
data to the packet. Data can also be page fragmented (not really sure what this
is) which allows for zero copy networking. `sk_buff` also contains `next` and
`prev` pointers. This is useful because it is often oragnized as a queue for
things like the socket receive queue, tcp send queue, nic transmit queue, etc.

`udp_send_skb`: Create the udp header. UDP specific operations.

`ip_send_skb`: Calls `ip_local_out` which calls `__ip_local_out` which runs the
packet through `netfilter` hooks. Eventually calls `dst_output` which is
indirect calling `ip_output`

`ip_output`: Prepares the packet for the outgoing network device, runs post
routing `netfilter` hooks.

`ip_finish_output`, `__ip_finish_output`, `ip_finish_output2`: Check cgroups
for the out device. Does IP packet header stuff like figuring out `MTU` ,
setting packet headers, etc. Set up for the link layer protocols like checking
if header has enough size to prepend, 

`ip_neigh_for_gw`: Finds the neighbor entry for the next hop.
n
`neigh_output`, `neigh_hh_output`: Sends the packet to the device layer.
Prepends the packet with a cached link layer header (this is the fast path). 

`dev_queue_xmit`, `__dev_queue_xmit`: Queues a buffer for transmition on a
device. The handoff to the device/qdisc layer. At this point, the packet is
already built and the output destination is set. There are two main paths,
after choosing a net device transmit queue. IF the queue has an enqueue method,
it enques with `__dev_queue_xmit`. If it doesn't, it takes the noqueue path
which is `dev_hard_start_xmit` which hands off to the driver's transmit
routine (this is common for loopback interfaces).

`netdev_core_pick_tx`: Picks the tx queue to put the packet into. This can be
with various strategies such as driver provided selection, flow hashing,
XPS/CPU based mapping, etc.

netdev transmit queues: A `net_device` strucut exposes one or more transmit
queues. NICs can actually have more than one hardware transmit queues as well.
`qdisc` sits before this net dev tx queue and makes decisions on the scheduling
of queues. Note that the qdisc queue isn't the actual hardware queue in the
NIC. It is a software queue that the kernel manages. The packet is queued into
the net dev transmit queue which `qdisc` manages and eventually pushes into the
NIC hardware queue.

`__dev_xmit_skb`: The core `qdisc` transmit helper. It decides whether the
packet can be sent to the network device immediately or whether in needs to be
enqueued into the qdisc. If bypass is not possible, it enqueus the packet. If
bypassing `qdisc`, it is passed directly to `sch_direct_xmit`. If not it is
enqueued. In both cases, the `__qdisc_run` and `qdisc_run_end` functions are
invoked.

qdisc: If a packet is enqueued, `__qdisc_run` is used to start the qdisc. qdisc
can also be run from the TX softirq `net_tx_action` which processes scheduled
output queues. `qdisc_run` may not drain all the packets in the queue. It is
possible for the packet that you queued to not be transmitted in the
`qdisc_run` case in which case it will be transmitted in the future by another
packet being enqueued or the `net_tx_action` softirq.

`sch_direct_xmit`: Tries to transmit directly.

`dev_hard_start_xmit`: Handoff between the generic network stack to the network
device driver. It receives a list of `sk_buff` and iterates through them and
calls `netdev_start_xmit` which is the driver transmit function.

`ndo_start_xmit`: The network device driver's transmit entrypoint. Network
device drivers register a table of callbacks called `struct net_device_ops`
that contain callbacks for things like registering, unregistering,
transitioning to the up state, doing `ioctl`, starting transmission. Not really
sure how this is setup and won't look into it here.

`igb_xmit_frame`: Now in the device driver, for our case the Intel `igb`
Ethernet driver. Won't go through the full stack trace of the device driver
side. I believe `igb_tx_map` is the key function where packets are being added
to the NIC hardware tx queue.

Device driver and NIC tx ring: The ring of descriptors that the NIC inpsects is
allocaed with DMA on system memory. The driver and the NIC each keep indices to
the next to use and next to clean in the descriptor ring. This is a producer
consumer relationship with the driver being the producer and the NIC being the
consumer. The driver takes each `sk_buff` and maps the `sk_buff->data` (the
actual packet data) with DMA so that the NIC can see it. It then records the
addresses in the tx ring buffer. This is how the NIC sees the packets it needs
to send. For the device driver to control the NIC and to communicate metadata
and share state, memmory mapped I/O is used. MMIO allows the CPU to write to
addresses that correspond to control registers on another device. An example
flow of communication between the driver and the NIC may look something like.

1. Driver maps packet data to DMA so NIC can see it.
2. Driver writes the address of the data into the ring descriptor
3. The device driver increments its next index pointer
4. The device driver writes the new tail index to the NIC register to indicate
   there is one more packet. This is done through PCIe MMIO
5. NIC reads the TX ring via DMA
6. NIC finds new descriptors and DMA reads the packet buffers
7. Transmits the packet
8. Fills in the status bits of the descriptor index that it just sent
9. The driver can free the `sk_buff` structure and the packet data

Why DMA for packets and not MMIO? It seems like MMIO is not used to transfer
packets because MMIO is bound by the CPU having to issue instructions to
trigger MMIO. There are also other issues such as MMIO memory types preventing
caching behavior which means main memory access every MMIO write. With DMA, the
DMA engine can issue reads and writes to main memory separate from the CPU.
This can also be done in parallel.

DMA Engine: A hardware engine that can read and write to system memory with
concurrent PCIe transactions. It is able to pipeline memory transactions for
high bandwidth transfer between the NIC and system memory.

## Other interesting things
- `likely`, `unlikely` macros are wrappers around the `__builtin_expect` GCC
functions that tells the compiler that the condition is likely going to be
`true` or `false` respectively. Compiler can optimize the code layout so that
the frequent path is the "fall-through" path.
- Indirect calls in Linux are runtime optimizations. It runtime dispatches
between `inet*` and `inet6*` depending on the socket.
`INDIRECT_CALLABLE_DECLARE` tells the compiler that a function is a known
possible target of an indirect function pointer call. `INDIRECT_CALL_INET` is
the actual call to the function depending on whether the function being passed
in is one of the functinos declared with `INDIRECT_CALLABLE_DECLARE`.
- UDP-Lite is UDP without checksumming
- `EXPORT_SYMBOl` macro makes a function visible and available for outside its
module 
