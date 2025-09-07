# Plan 9 From Bell Labs

## Summary
Plan 9 is a system built from various commercial hardware that is joined
together to create one UNIX like machine. Many principles from UNIX are applied
but the system is designed from scratch to address the issue of UNIX machines
ot working well together when interconnected.

## Motivation

At the time of this paper, UNIX workstations were the crazy and much attention
was devoted to personalied computing on commodity hardware. However, this meant
large centralized computing infrastructure was overlooked. This paper addresses
UNIX's main challenges for such infrastructure, namely
- The difficulty of making multiple UNIX machine act as a single time shared
machine
- Design flaws in UNIX, especially for subsystems introduced after its design
like graphics and networks

The idea of Plan 9, the system proposed in this paper, was to "build a UNIX out
of a lot of little systems, not a system out of a lot of little UNIXes". The
paper describes a time sharing system that uses different computers for
different tasks. There were some principles from UNIX that could be applied to
Plan 9 but the system was built from scratch.

## Design

Plan 9 is based on three principles
- Resources like files, processes, I/O devices are accessed like files in a
hierarchical file system
- The 9P protocol is used to access these resources
- Disjoint hierarchical file name spaces from different services (such as the
file system service, the CPU service, the datakit service) are joined together
to build a single hierarchical file name space

A Plan 9 installation is a distributed system of computers where each computer
provides a particular service. Some computers are dedicated compute, some offer
file storage and some offer interfaces to the rest of the system. Plan 9 is
meant for multiple users to use the system, however each user has a
personalized view of the system. This is possible by giving local personal
names to resources for each user. TLDR, you and I can say "my house" but we
will both mean different houses. Similarly, a user can inspect /proc to see
their processes but every user will see a different view of processes running
on the Plan 9 system.

Plan 9 extends the notion of files as interfaces to everything, even for things
like I/O devices, processes, network interfaces.

## Command Level View
Users interact with the system through machines dedicated to serving a "window"
system. Each window is private to itself and changes made within a window does
not apply to other windows. Each window has a private view of the system
resources. The window system (8 1/2) controls the window through the
`/dev/mouse`, `/dev/bitblt` , and `/dev/cons` files. 

## The File Server
The file system is only accessible through the network and does not run any
user processes. It runs a fixed set of routines that provides the needed
operations on the file sstem. The file system is a single tree with subtrees
for sepcial purpose systems like temp storage and backup. The most interesting
part of the file system is that magnetic disk is a cache for the write once
read man (WORM) storage. Every morning, the file system is dumped into a WORM
device allowing users to have read only access to historic versions of the file
system. The dump can also be used as a system backup in case of catastrophic
failure but this isn't recommended as the recovery results in a very slow
system taking days to regain full performance. An interesting note with WORM
devices is that Plan 9 considers WORM as "unlimited storage". This is because
even after five years of use for 50 people, they only filled 65% of the device
and during that time, the manufacturer made new devices with better capacity.
"Technology has created storage faster than we can use it".

## Unusual File Servers
Services are interfaced via files in Plan 9. For example, the 8 1/2 window
system interfaces with the user through a set of files in the `/dev` directory.
What's special about Plan 9 is that while all window systems use this `/dev`
directory and it looks like that locally, in reality, every window has a
distinct set of files in `/dev` so even though you interface with the same
`/dev/cons` in two different windows, they are actually different consoles.
This is possible with the private namespacing in Plan 9. File transfer in Plan
9 is not done through `ftp` command but through a `ftpfs` that internally uses
`ftp` on behalf of the user. Another special server is `exportfs` which allows
a portion of a user processes's namespace to be made available to other
processes. This is used for when you want to access a specific resource like a
network interface in the Helix central server `import helix/net`. The opposite
of import would be the `cpu` command that starts an `exportfs` server locally
and mounts it to a newly created process on the server.

## Configurability and Administration
It is possible to run Plan 9 on a single laptop PC or a datacenter like
multiprocessor system with dedicated processors for compute and file servers.
Central servers centralize the administration of the system as well. One main
server holds all of the system files where a program has a single copy for each
architecture allowing portability. There is also only a single user database so
there is no need for synchronization. The central server in Plan 9 is what
allows it to be "simple" and maintain the system without the need for elaborate
synchronization mechanisms between different components.

## Portability and Compilation
Plan 9 is portable across various architectures because different components
will inherently run different processors and thus compilres. This is done with
cross compilation where Plan 9 uses a specific compiler for each architecture.
The build system in Plan 9 (`mk`) is able to create object files for each
architecture type without running on the native architecture. For example, you
would be able to compile object files for SPARC on an x86 processor.
Communication between processes is done as much in text format to avoid byte
ordering problems. When byte streams are absolutely necessary, multie byte
values are encoded with a pre defined encoding.

## Parallel Programming
Plan 9 doesn't have threads. Instead, its processes are cheap to create and
schedule so all parallel programming is done through processes instead of
threads. Alef is used for parallel programming instead of C. This is because
the interface needed for parallel programming is different for Plan 9 and it is
easier to have principles of parallel programming backed into the language
instead of at the kernel level. New processes are created with `rfork()` which
takes in a bitmap that specifies which resources (fd, memory, name space,
signals) of the calling process should be shared with the child process. For
process synchronization, `rendezvous` system call is used. This takes a tag and
the process sleeps until another process calls with the same tag. This is
enough to implement synchronization routines and various features like
communication channels, queuing locks, multiple reader/writer locks, etc. Spin
locks are provided in architecture dependent libraries since some processors
have different ways of acquiring hardware locks.

## Implementation of Namespaces

Each namespace is essentially a file tree. Subtrees can be added and removed
using the `mount` and `unmount` system call. Using `bind`, multiple directories
can be stacked into a single node on the file tree which is called a union. A
union is essentially two subtrees stacked on top of each other, a concatenation
of two directories, and when looking for an entry in a union directly, all
constituent directories are searched. 

Brief description of 9P protocol: Protocol that joins together the components
of Plan 9 without having to invent a new protocol for each use case 

## Network and Communication Devices
Interfacing with the network is done through the file system. For example, to
interface with TCP, you would use the `/net/tcp` directory that contains files
for TCP functionalities like `connect`, and `listen`. A specific daemon process
is used to listen for new connections called the listener. Applications need
not know all the addresses of other services. Instead, a connection server
contains the addresses of services and applications make requests to the
connection server for the address that they need. 

## IL Protocol
9P needs a reliable delivery protocol underneath it to function. TCP is not
suitable because it is a byte stream abstraction and 9P has discrete messages
that needs to delimite messages. Internet Link is the protocol designed for
reliable delivery in Plan 9 that implements reliable transfer of sequenced
messages without flow control because each process can only have one
outstanding 9P message at a time.

## Overview of Authenticatio
Users are authenticated by encrypting and decrypting special messages called
challenges. Users type in the DES key in the console and servers store it in
some permanent storage medium. The client and the server exchange challenges.
Then both parties contact the central authentication server that maintains a
databse of keys for users. The authentication server returns tickets that
contain a conversation key encrypted with the queriers key. Each party uses the
conversation key to encrypt each other's challenges. If the challenges match
from what you sent, the other side is authenticated.

## Special users
There is no system wide super user in Plan 9. Each service has its own
administrative user for maintaining that service.

