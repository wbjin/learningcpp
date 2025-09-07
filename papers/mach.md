# Mach: A New Kernel Foundation for UNIX Development

## Introduction
Multiprocessor kernel that includes
- Separation of process abstraction to tasks and threads with the ability to
execute multiple threads within a task
- Virtual memory design that provides large, sparse virtual address space
- Copy on write
- Copy on write and read write memory sharing between tasks
- Memory mapped files
- User provided backing store objects and pagers

## Design an extensible kernel
UNIX used file descriptors to refer to objects and allowed simple operations
such as read, write, and seek on this objects. However, with changing hardware
and different technology such as sockets and sahred memory, the authors of this
paper concluded that a simplification that allows the UNIX model of consistent
interfaces was needed. Mach simplifies these complex resources by providing
small set of primitives that allow the object to be placed in the network and
complex systems to be built on top of. The Mach kernel supports four abstractions, 
1. Task: Execution environment that threads are a part of. Basic unit of
resource allocation, containing its own virtual address, file descriptors,
ports. The equivalent of UNIX process.
2. Threads: Basic unit of CPU utilization. All threads within a task share the
task's resources.
3. Port: Communication channels to interface with the kernel to refer to
objects
4. Message: Collection of data objects used in communication between threads

Operations on objects are performed by send messages to ports. The Mach kernel
is essentially a server and the user task a client asking it to fulfill system
capabilities on its behalf. 

## Tasks and Threads
UNIX processes have too much overhead and typical server applications that fork
to create a new process for each client don't need full blown processes to
handle requests. Users have been using coroutines that allow multiple contexts
on one process but in a multiprocessor, this process will still be scheduled on
one procsessor because the scheduler has no knowledge of the coroutines. Mach
abstracts processes into tasks and threads. A task is a collection of system
resources like virtual address space and ports while threads belong to a task
and share the same address space. In multiproessors, multiple threads can be
scheduled on different processors allowing a single process to be scheduled on
different processors. Operations on tasks and threads are done through sending
messages to a port representing that task or thread. 

## Virtual Memory Management
Mach's virtual memory allows allocation and deallocation of regions of virtual
memory, setting protcetion on page basis, and specifying inheritance of pages
on forks. When a task forks, it creates a child task with its own address
space. If the inheritance is shared, the pages specified are read/write shared
for both the parent and the child process. If pages are specified as copy, the
pages are copied into the child's address space via copy on write mechanism.
Inheritance of non means that the child's address space is left unallocated.
Protection can also be specified on a per page basis where each page has a
current and maximum protection which are combinations of read write and execute
bits. When virtual memory is created, a task specified for handling page faults
is created. When a page fault occurs, the kernel translates the fault to a
request to that task.

## Interprocess Communication
Mach's IPC is done through ports and messages. A port is a protected kernel
object that messages can be placed in. Ports represent a specific resource. To
access a resource, programs would have to send a message to the port associated
with the resource. Messages are sent and received through RPC like interfaces. 
