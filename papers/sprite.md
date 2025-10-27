# The Sprite Network Operating System

## Introduction
Sprite is a network operating system designed for multiprocessor systems. The
driving force behind Sprite are
- Increasing distribution of systems: Many workstations at the time consisted
of personal workstations for computing and large time shared machines for
workloads that couldn't run on personal workstations. However, this personal
workstation environment suffers from poor performance and difficulty of
administration. Spirte is designed to abstract this away as much as possible.
- Increasing physical memory size: At the time of the paper physical memory
ranged from 4 to 32 MB with expectations of 100 to 500 MB memory soon. The
Sprite authors hoped that this lare memory will allow commonly accessed files
to reside in memory allowing file caching in the Sprite system
- Rise of multiprocsesor workstations: Many research oranizations are creating
multiprocessor systems. Sprite is designed to take advantage of a
multiprocessor environement

Sprite was designed to be as close as possible to Unix with the kernel calls
being similar to 4.3 BSD. However, Sprite introduces new features that takea
advantage of the hardware trends in a transparent network file system,
sharablewritable memory between processes, and process migration across
workstations. While the kernel interface stays consistent, the implementation
was changed drastically.

## Application Interface
The kernel interface of Sprite is the same as the Berkeley versions of Unix
with the only changes being the one mentioned above.

Sprite's file system has network transparency meaning file accesses stay the
same as time sharing, single machine file systems. The distribution is
invisible to the user. Existing file systems, even the most recent systems like
Andrwe File System and NFS, still have nontransparent aspects. For example, in
AFS and NFS, only a portion of the file system hierarchy is shared. Each
machine still has a private partition that is only accessible within that
machine. AFS and NFS also don't permit applications running on one machine to
access I/O devices on other machines. Locus seems to be the only other file
system that has complete transparency. Sprite is completely transparent. A
single file system hierarchy is uniformly accessible across all machines and
the location that a file is stored is not important in normal operations.
Sprite also provides trasnparent I/O device access with devices being
represnted as special files. However, Sprite also allows any process to access
any device regardless of physical location.

Shared memory on Unix has evolved to allow read write memory sharing between
processes. Shared memory between multiple processes is desirable because it is
the most natural way to program applications that consist of mostly independent
subactivites. A shared address space allows these processes to communicate
easily. Secondly, with the rise of multiprocessors, truly concurrent
applications running on different processors are possible. To maximize
parallelism, the communication between these concurrent executions must be as
fast as possible and shared memory provides the fastest possible communication.
Sprite allows shared memory by allowing a parent's data segment to be shared
with a child process on a fork. The differnet processes can synchronize using
mutual exclusion instructions like test and set on this shared memory region.

In a networked environment with muiltiple workstations, many machines will be
idle at any given time. To use this idle compute, Sprite provides a new kernel
call called Proc_Migrate that moves a process or group of processes to a
differnet machine. Processes that are sharing memory must migrate together.
Sprite keeps track of machine idleness and selects a target for the migration.
Process migration is transparent to the migrated process and the user. The
migrating of process will allow users to migrate processes in a similar way to
users putting processes in the background. Second, PMake which is a new version
of the Unix Make allows multiple recompilations concurrently and process
migration can be used to offload compilations to idle machines. This idea is
not entirely new to Sprite. rsh in BSD and rex in Sun allows remote invokation
which allows new processes to be created remotely. However, process migration
allows a procses to be migrated at any time. It is important that Sprite
migrates processes transparently. When migrated, a process' state is the exaxct
same with files, working directory, device access, environment variables and
everything else being the same. It will also appear to the user as if it is
still running on the home machine.

## Basic Kernel Structure
The biggest change to the kernel structure that Sprite introduces is a
multithreaded kernel to run on multiprocessors and a RPC facility for remote
invokations across the network. Unix at the time was single threaded meaning if
any procss invoked the kernel, it acquired a lock and released once it returned
to user state or slept. Processes that are kernel mode cannot be preempted.
This simplifies kernel implementation but does not work well for a
multiprocessor environment where many processes can contend for the kernel
lock. In contrast, Sprite is multithreaded meaning multiple processes can be in
kernel mode at the same time. Instead of one single global lock, each module or
data structure has a lock allowing processes to run in kernel mode parallely as
long as they do not touch the same critical region. This does have its downside
as it increases implementation complexity and increases the lifetime of each
kernel call.

Sprite uses RPCs for kernels on different machines to talk with each other.
Each RPC has two stubs, a client and a server stub with a transport facility on
both sides. The client stub copies its arguments to a request message and
returns values from a result messae. The server stubs passes arguments from an
incoming message to the underlying procedure and packages results into a result
message. The RPC implementation also contains a transport that delivers
messages across the network and assigns incoming RPC requests to kernel
processes that execute the server stubs and call the underlying procedure. The
transport protocol used for these procedures was custom to Sprite with implicit
acknowledgements and fragmentation. Each message needs an acknowledgement and
it is sent again if no ACK comes within a time frame. To reduce the number of
network messages, Sprite piggybacks ACKs on request or response messages where
a request or response serves as an implicit ACK of the previous request or
response. Because the MTU of an Ethernet cable is only 1500 bytes,
fragmentation is needed for large blocks of data. If a request or reply does
not fit in a MTU, the transport breaks it into multiple packets that is
transmitted altogether. The receiving RPC transport reassembles these fragments
and a single ACK can be used for all of the fragments.

Measurements of the RPC show that the smallest RPC messages take around 2.8
milliseconds with an additional 1.2 milliseconds for each KB of data. This is
nice to see as it means latency increases linearly and theres not much overhead
being added with larger messages. Throughput however does not grow linearly and
caps out around 700 KB/s with large amounts of data. Throughput is drastically
reduced with smaller messages which is to be expected. That being said, user
visible latency and throughput is slightly worse than depcited.

## Managing file name space - prefix tables
To users, Sprite's file system is a single hierarchical file system just like
Unix. To adminstrators, the file system is a collection of domains. Each domain
contains a tree structured portion of the file system by overlaying the leaves
of some domains with the roots of other domains. This functions kind of like a
file system being mounted on a parent's leaf node. The challenge of a
distributed file system is keeping track of the domain structure and handling
file names that span multiple domains. In Unix and derivatives like NFS, mount
tables are used to keep track of domains. These mount tbables are established
by reading a config file at boot time which makes it difficult to respond to
configuration changes. Sprite uses a dynamic approach using prefix tables. Each
client machine maintains a prefix table where each entry corresponds to a
domain. The entry contains the name of the top level directory of that domain,
the server that the domain is located on, and a token to pass to that server to
refer to the domain. 

For looking up a file, the kernel matches the name against all entries in the
prefix table and chooses the entry with the longest prefix match. The client
will then strip the prefix match and send a request to the server containing
that domain with the token from the prefix table entry. The server uses the
token to locate the root directory of the domain, searches for the remainder of
the file, and replies with the token identifying that file. The client can then
use that token to issue read, write, and close requests. In cases where file
names cross a domain boundary, the server in the prefix table may not be able
to fulfill the request. In this case, the server processes as much of the file
name as it can and returns a new name to the client instead of a token. The
client takes the new name, checks with its prefix table, and sends it to a new
server. This process is repeated until the name is found. This approach allows
traffic to be reduced on the root server since a lot of the file look ups
involve absolute paths, not every request has to go to the root server.

Prefix tables are created dynamically and updated automatically when the system
configuration changes. To add a new entry to the prefix table, a client
broadcasts a prefix name to all servers. The server storing the domain replies
with its address and the token corresponding to the domain. The client uses
this to create a new prefix table entry. Initially, the client starts with an
empty prefix table and broadcasts to find the entry for the root directory. As
more files are used, it adds entries to the prefix table. Files at the mount
point of domains are called remote links. Remote links operate similar to
symbolic links where the file stores the absolute name of itself. Whenever a
new remote link is encountered in a file name lookup, the server returns the
prefix up to the remote link and the remainder that needs to be looked up. The
clinet uses this to braodcast and add an entry for the remote link. Remote
links don't store any network addresses but rather is just an indication of a
domain crossing point. It is possible for the client's request to be rejected
or timed out. In this case, the client invalidates the prefix entry and
broadcasts again to find the server that contains the domain.

## Managing file data - client and server caches
Sprite's file system has a large cache of recently used file blocks stored in
the main memory of the client and server. Each client and server workstation
maintains a cache of recently accessed file blocks (rather than entire files).
The kernel first checks its cache, then the disk or the server. On a cache
miss, the block is retreived from local disk or the server and is added to
cache with replacements using an LRU policy. Sprite uses delayed writes where
the data is written to the cache and returned to the appliation instead of
fully writing to the disk. The block is written when it is ejected or after 30
seconds. This comes at the cost of some data loss but much higher performance.
Sprite does expose a system call to to flush one ore more blocks to disk for
applications that do need the feature.

Cache consistency is a hard problem with network file systems but it comes at
the cost of some reads being stale if multiple clients change the same file
block. Sprite guarantees consistnecy however, where the read system call always
returns the most up to date data for a file. The authors considered two cases,
the first is sequential write sharing where a file is modified by one work
station and later read by another workstation but never open at the same time.
The second case is concurrent write sharing where one workstation modifies a
file while another has it open. Sprite uses version numbers to handle
sequential write sharing. When a client opens a file, the server returns the
file's current version number which the client compares to the version number
associated with its cached blocks. If different, the file must have been
modified and the client invalidates the cache entries for that file and
repopulates them as they are needed. Sprite's file system also has cachin which
means that the latest change may not be in disk. The server deals with this by
keeping track of the last writer for each file. When a client other than the
last writer requests an open, the server forces the last writer to write all of
its direty blocks back to the server disk. For concurrent write sharing, Sprite
disables client caching for that file. When the server receives an open request
that will cause concurrent write sharing, it flushes dirty blocks back from the
last writer and notifies all other clients that have the file open to not cache
the file. One main disadvantage to Sprite's cache consistency is substantially
slower file access when caching has been disabled. However, measurements froma
different paper show that files tend to be open for only short periods and are
rarely write shared. Second is the fact that it relies on the server being
notified whenever there is a file being opened or closed. This limits
optimizations like name caching where clients open files without contacting the
file's server. A benchmark from a differnet paper however suggests that this
would provide little performance benefit. Sprite's caching mechanism ensures
consistency. For correct synchronizatino, applications must use the file lock
system call.

To measure the benefits of caching, several file intensive benchamrk programs
were run with serveral clients and a single file server was used for all client
I/O and paging traffic. Important to note that these benchmarks don't test file
sharing which is delegated to another paper. The authors showed that diskless
workstations with caching can achieve within 1 to 12 percent of workstations
with local disks while diskless workstations without caching runs 10 to 40%
slower than workstations with disks. It also shows that client caching can
reduce network traffic by a factor of four or more and without it, the authors
hypothesize that Ethernet's 10 Mbps bandwidth will be a major bottleneck.
Client cachin also allowed server load to reduce by a factor of 2 and suggests
that a single server can support 10 or more active clients without excessive
performance degradation. The Andrew benchmark completed 30% faster on Sprite
than on Sun's NFS. Comparison with NFS is done in a separate paper.

## Virtual memory
Sprite's virtual memory is traditional in that it uses many of the same
techniques in Unix. For example, a variation of the clock algorithm used for
page replacement. The paper only really talks about the aspects of virtual
memory that deviates from Unix to better use the network and larger physical
memory. Sprite uses ordinary files instead of special swap files to make
process migration easier. Sprite also allows trade off between virtual memory
and file cache. 

In Unix, a special partition in disk is used as the backing store of pages. In
network Unix systems, each machine has its own private disk partition. In
Sprite, ordinary files stored in the network file system is used with separate
backing files for each data and stack segment. This simplifies the
implementation of paing by reusing existing file system. It also allows
flexibility with process migration and file allocation on local disks. Remote
backing files are cached just like all other files. Measurements show that a
client can read random pages from a file in the server's cache faster than from
a local disk which means server with lare caches may provide better paging
performance than a local disk. This takes advantage of the fact that CPU and
network speeds increase much faster than disk speeds. 

Program data and code segments are kept in memory until evicted by the clock
algorithm. This is called a sticky mechanism and it reduces paging for
frequently accessed programs.

Double caching is a potential issue because the virtual memory system is a user
of the file system. Pages being read from backing files could end up being
cached in the file cache and the virtual memory page. To avoid this, the
virtual memory system bypasses the local file cache when reading and writing
backing files. Backing files are still cached in the servers which allows the
server's main memory to be an extended main memory for clients.

Virtual memory and the file cache conflict as they both use the limited
physical memory. Sprite allows the file cache to grow and shrink in response to
changing demands on the machine's virtual memory and file system. The two
manage separate pools of physical memory pages. Whenever either module needs
more apges, it compares the age of the oldest page with the age of the oldest
page in the other module and replaces whichever one is older. 

## Process migration
Sprite's process migration differs from existing systems in two major ways. The
first is the way virtual memory is transferred and the second is differences in
the way migration is made transparent to the migrated process. A simple process
migration approach is to freeze the running process, migrate all of its state,
and unfreeze it at the destination machine. Virtual memory transfer is the
dominant cost. Various techiniques such as precopying where the process
continues executing while memory is transferred or a lazy approach where pages
are only migrated on page faults exist. Sprite's file backing system simplifies
migration. The old machine pages out the process' dirty pages and transfers
information about the backing files to the target machine. Pages get reloaded
in the process' new machine on demand which means the migrating process only
needs to be freezed for the duration of the dirty page flushes. For
transparency, Sprite assigns each process a home node which is the machine that
the process was created on. Whenever a process invokes a kernel call whose
results are machine dependent, it is forwarded to the home node and executed
there.
