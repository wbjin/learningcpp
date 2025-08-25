## Multiple Access Links and Protocols

Pages 461-477 Section 6.3

There are two types of network links, point to point and broadcast. Point to
point consists of a single sender and a single receiver on both ends of a link.
Broadcast can have multiple senidng and receiving hosts all connected to the
same, single, shared broadcast channel. When any node transmits a frame, all
other nodes on the link receives a copy. Because multiple nodes are on the
link, the link layer has to coordinate the access of multiple senidng and
receiving nodes on a shared channel. Multiple access protocols, protocols that
allow nodes to regulate their transmission on the shared channel allow hundreds
and thousands of nodes to communicate.

The main problem over communicating over a single channel is that when multiple
nodes transmit frames, these frames can collide making it impossible to
understand what was sent on the channel. Access protocols are categorized into
three categories, channel partitioning, random access, and taking turns. 

### Channel Partitioning
Time divisioned multiplexing (TDM) and frequency divisioned multiplexing (FDM)
are different ways to partition a channel. In TDM, time is divided in time
frames where each host is given a specific time slot that it is allowed to
transmit data. Usually, this time is set so that each node can transmit a full
frame. This allows nodes to avoid collision and to share the bandwidth equally.
However, a node would have limited bandwidth of $R/N$ where $R$ is the channels
capacity and $N$ is the number of nodes even though not all nodes need to
transmit. Furthermore, a node must wait until its turn to transmit even when it
is the only one sending. FDM on the otherhand divides the $R$ bps channel into
different frequencies each with a bandwidth of $R/N$ and assigns each node a
frequency to transmit on. FDM is also limited to $R/N$ bandwidth even though
there may be one node transmitting at a time.

A third partitioning protocol is code division multiple access (CDMA). CDMA
assigns different codes to each node. Each node uses its unique code to encode
the data and if the codes are chosen carefully, CDMA networks can transmit
simultaneously and nodes will still be able to understand the messages.Vgq

### Random Access Protocols
In this protocol, transmitting nodes always transmit at the full rate of the
channel, $R$. When there is a collision, each node involved in the collision
repeatedly retransmits its frame until its frame gets through without a
collision. Each node retransmits its frame after a random delay that are
independently chosen at each node. 

Slotted ALOHA: Slotted ALOHA is a random access protocol. In ALOHGA, time is
divided into slots of $L/R$ where $L$ is the size of frames. This means that
each slot is the time it takes to transmit exactly one frame. Nodes only
transmit frames at the beginning of a slot and all nodes are synchronized so
that they know when a slot begins. Then, each node does the following:
- When there is a new frame to send, wait until the beginning of a new slot and
transmit the entire frame
- If there isn't a collision, successful transmission and don't need to do
anything
- If there is a collision, node retransmits in a subsequent slot with
probability $p$ until the frame is transmitted without a collision where $p$ is
a probability between 0 and 1

When there are collisions, slotted ALOHA has two types of wasted slots: when no
node transmits or when more than one node transmits. The probability of an
unwasted slot, that is only one node transmits, is $p(1-p)^{N-1}$ where $N$ is
the number of nodes and $p$ is the probability of retransmission. Because there
are $N$ nodes, the probability of any one node succeeding is $Np(1-p)^{N-1}$.
For maximum efficiency, you have to solve for $p$ with a given $N$. For an
arbitrary large number of nodes, we can take the limit of the equation as $N$
approaches infinity. Given this $p$ value, the maximum efficiency is
$\frac{1}{e} = 0.37$.

Carrier Sense Multiple Access: ALOHA's weakness is that the decision to
transmit is made independent of the activity of the other nodes. A node
transmits even if another is currently transmitting nor does it stop
transmitting once there is a colliding transmission. However, we can do two
things:
- Listening before transmitting: Carrier sensing, a node listens on the channel
before transmitting. If a frame from another node is currently being
transmitted, then it waits until no tramissions are detected
- Stopping: Collision detection, if a transmitting node detects a conflict
while transmitting, it just stops and waits for a random amount of time until
it transmits again

These two rules are embodied in the CSMA and CSMA/CD (with collision detection)
protocols. Even though nodes wait until there are no conflicts, conflicts can
still happen because frames take a non-trivial amount of time to propagate
through the channel. A node might start transmitting at time 0 and it could
take until time 3 for it to get to a node on the channel. 

CSMA/CD: Collision detection is when a node can detect that while it is
transmitting, there is another conflicting transmission by another node. If a
node detects this, it will stop transmitting to not waste bandwidth and time.
It will then wait a random amount of time until retransmits. This timeout is
determined by binary exponential backoff. For $n$ collisions, a node chooses a
random from 0, 1, 2, ... to $2^{n-1}$ where the max value of $n$ is capped at
10. The efficiency of CSMA/CD is approximated as $\frac{1}{1 +
5d_{prop}/d_{trans}}$ where $d_{prop}$ is the max time it takes a signal to
propagate between any two nodes and $d_{trans}$ is the time to transmit a max
sized frame. As $d_{prop}$ approaches 0, the efficiency approaches 1 and as
$d_{trans}$ approaches infinity, efficiency also becomes 1.

### Taking Turns Protocols
With ALOHA and CSMA/CD, it doesn't guarantee the property where when $N$ nodes
are active, bandwidth is split equally amongst each other. Taking turns
protocols address this

Polling protocol: One node is designated as a master node it polls each of the
nodes in a round robin fashion. The master node tells the other nodes one a
time that it can transmit. The Bluetooth protocol is an example of a polling
protocol

Token passing protocol: A special purpose frame known as the token is exchanged
among nodes in some fixed order. When a node receives a token, it holds it
while it transmits some maximum number of designated frames and then passes on
the token.
