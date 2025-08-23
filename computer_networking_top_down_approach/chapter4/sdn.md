## Generalized Forwarding and SDN

Pages 353-359 Section 4.4

In traditional, hardware implemented routing, the IP address match and action
was simply destination based by inspecting the destination address in the
header. Instead, we can change the match to be across multiple heaer fields not
just destination and action to be more sophisticated like sending it to
multiple output ports, load balancing packets across multiple outgoing
interfaces, rewriting header values, purposefully blocking/dropping packets,
etc. The routers pased on only the destination in the IP packet header is more
accurately a packet switch. By using software defined networking where the
match + action at routers are implemented by remote controllers, you can do
much more with routing packets. An example is OpenFlow, a standard for match +
action based forwarding. 

Each entry in the match + action forwarding table is known as a flow table. It
contains
- Set of header fields that packets are matched to
- Set of counters that are updated as packets are matched to flow table entries
- Set of actions to be taken when a packet matches a flow table entry

### Matching
OpenFlow allows matching to be made on selected fields from three layers of
protocol headers (link, network, and transport layer). This means OpenFlow
enabled device can perform as network switches ad well as routers. Packets can
also match to multiple flow entries, each flow entry can be assigned a priority
and the highest entry match will be chosen. 

### Action
Actions that packets match to can be
- Forwarding: Forward packet to a particular output port, broadcast to all
ports, or multicast over selected set of ports
- Dropping: No action means to drop the packet
- Modify: The values in the header fields can be overwritten (except for IP
protocol field) before forwarding to chosen output port
