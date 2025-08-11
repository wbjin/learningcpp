## DNS

Pages 122-135 Section 2.4

Domain name system is a distributed dsatabase that runs at the application
layer to allow hosts to query IP addresses by hostnames ie a service for
translating human readable hostnames to IP addresses.

DNS is a distributed, hierarchical system with different kinds of DNS servers.
- Root DNS servers: 13 servers scattered across the world that provides IP
addresses of TLD servers.
- Top Level Domain (TLD) servers: Top level domains are domains like .com,
.edu, .us. A TLD contains IP addresses for authoritative DNS servers.
- Authoritatie DNS servers: A DNS server managed by an organization that is
publicly accessible through the internet. Contains IP addresses of hostnames or
could be another authoritative DNS server as well for example dns.umich.edu
contains the IP for cse.umich.edu
- Local DNS servers: DNS server provided by ISP. When a host connects to an
ISP, ISP provides addresses for one or more of its local DNS servers through
DHCP. Host sends DNS query to local DNS server and local server acts as a proxy
and queries the DNS server hierarchy.

An example flow of a hostname resolving to IP is
1. Host queries local DNS server for the hostname cse.umich.edu.
2. Local DNS forwards it to one of root DNS server and root DNS server sees the
   .edu suffix.
3. The root DNS server returns the IP address of one of the TLD servers
   responsible for .edu.
4. Local DNS server queries the TLD server.
5. TLD server sees the umich.edu suffix and returns the IP address of the
   authoritative server responsible for umich.edu.
6. Local DNS erver queries the authoritative server.
7. Authoritative DNS server contains the IP for the hostname cse.umich.edu so
   it returns the IP address for it to local server.
8. Local DNS server returns the IP to host.

There could also be more intermediary servers between the TLD and the
authoritative server. The authoritative server could also link to another
authoritative server. The following is an iterative approach to DNS resolution.
There is also a recursive way of DNS resolution where instead of the root, TLD,
and authoritative servers returning the IP address of the next DNS server, it
actually queries that DNS server for you. In this case it would look like
1. Host queries local DNS server for the hostname cse.umich.edu.
2. Local DNS forwards it to one of root DNS server and root DNS server sees the
   .edu suffix.
3. The root DNS server queries TLD server
4. TLD server queries the authoritative server.
5. Authoritative server returns the IP address of the hostname.
6. TLD server forwards the IP address to the root server.
7. Root server returns the IP address   back to the local DNS.
8. Local DNS server returns the IP to host.

DNSs often cache hostname resolutions. A cached resolution doesn't have to go
through the hierarchy of DNS servers and thus can be fetched faster. The
resolution stays in the cache for a fairly long period of time, around two
days.

DNS resource records are tuples of `(Name, Value, Type, TTL)` and there are
four different types.
- Type=A: Name is a hostname and Value is the IP address of the hostname. Type
A records resolve hostnames to IP addresses.
- Type=NS: Name is domain and Value is DNS for that domain
- Type=CNAME: Name is an alias hostname and Value is the canonical hostname. An
alias hostname could be a shorthand hostname (google.com) to the
canonical/actual hostname of (relay1.google.com)
- Type=MX: Name is hostname of a mail server (gmail.com) and Value is the
canonical name of the mail server (mail.google.gmail.com).

DNS only has query and reply messages which both have the same format.
Header section in the following order:
- 16 bit unique number that identifies the query. This is copied into the
response.
- 16 bit flag section. Flags for query or response, opcode, whether DNS
server responding is authoritative or not, whether the client wants desires
that DNS server perform recursion, response code
- 16 bits for number of questions
- 16 bits for number of answers
- 16 bits for number authority RRs
- 16 bits for number of additional RRs

The questions section contains
- QNAME is the hostname or domain name, QTYPE is the record being requested
like A or AAAA or CNAME, etc, QCLASS is the the class of the query for example
IN for internet

The answer section contains
- RRs that directly answer the query where each RR contains name, value, type,
class, and TTL. 

The authority section contains
- RRs for authoritative name severs for the queried domain when a DNS can't
directly answer the question but knows which nameserver are responsible for the
domain

The additional information section contains
- Supplementary information that might be helpful like the IP addresses of the
NS in authority section

DNS uses UDP so the max size of a message is the UDP standard 512 datagram
limit. 

DNS record insertion
You register a new IP address/hostname at a registrar like GoDaddy. You have to
specify two authoritative severs that contain the record for your hostname. The
registrar will create a NS and A record for each of the authoritative server
you mentioned and enter it into the TLD servers. For example, it would look
something like
```
(aaa.com, dns1.aaa.com, NS) <- Authoritative nameserver
(dns1.aaa.com 121.121.111.111 A) <- IP of authoritative nameserver
```
