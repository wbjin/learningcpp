## Intra-AS routing in the Internet: OSPF

Pages 396-399 Section 5.3

In the internet, routers are organized into autonomous systems where each AS
has a group of routers that are under the same admnistrative control. Routers
within an AS all run the same routing algorithm and have information about each
other.

### Open Shortest Path First

OSPF and IS-IS are intra-AS routing for the internet. OSPF is a link state
protocol that uses flooding of link state information and a Dijkstra's least
cost path algorithm. The link costs is up to the network administrator. It
might all be set to 1 for minimum hop routing or may be set to inversely
proportional to link capacity in order to discourage traffic from using low
bandwidth links.

In OSPF a router broardcasts its link's state (directly connected links)
whenever there is a change in the link. It also broadcasts it periodically
(typically every 30 minutes). OSPF is encapsulated by IP with an upper layer
protocol of 89. OSPF contain the following features and more:
- Security: Exchanes between OSPF routers can be authenticated preventing
malicious routers. 
- Multiple same cost paths: When there are multiple same cost paths, OSPF
allows multiple paths to be used
- Unicast and multicast routing: Can use multicast for broadcast
- Hierarchy: Routers within an AS can be grouped and each group can run its own
OSPF routing and broadcast among themselves. One of the groups is configured to
be the backbone that routes traffic bewteen the other areas in the AS.
