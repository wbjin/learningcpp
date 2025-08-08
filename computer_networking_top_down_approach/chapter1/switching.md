## Packet vs Circuit switching

Packet switching is transmitting data as packets where each packet is
transmitted to the next closest hop by a switch using a routing table. Most
packet switches are store and forward meaning they wait until they receive the
full packet before sending it to the next link. The delay of a $N$ link ($N-1$
routers) path for transmission of one packet is $N\frac{L}{R}$ where $L$ is the
number of bits in the packet and $R$ is the transmission rate of the wire
(bandwidth). This ignores propagation delay. If you are sending $P$ packets, it
is $(P-1+N)\frac{L}{R}$

Circuit switching is determining a path before sending any data out and and
reserving that path to send a message. This path is called a circuit and the
bandwidth of this circuit is determined in frequency division multiplexing
(FDM) or time division multiplexing (TDM). In FDM, different frequencies for
different circuites are used. For TDM, time is divided into frames and each
frame is divided into slots where a circuit and use a slot. For example, if you
have a link with bandwidth 1.536 Mbps and 24 slots in a time frame, each
circuit has a transmission rate of 1.536Mbps / 24 = 64 kbps.

Circuit switching is considered wasteful because you can have idle circuits not
transmitting data that takes away from other circuits. With packet switching,
you can always send at maximum transmissions rate if there are no queueing
delays. With circuit switching you have determinstic packet routing though.
