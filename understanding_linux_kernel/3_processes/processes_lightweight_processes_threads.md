# Processes, Lightweight Processes, and Threads

A process is an instance of a program in execution. It is the collection of
data structures that describe the execution of the program. From the kernel's
point of view, a process is an entity to which resources such as compute,
memory can be allocated. Linux uses lightweight processes for multithreaded
applications. In the past, threading was done in user space the kernel treated
multiple user level threads as one thread. Lightweight processes share the same
resource like address space, file descriptors, etc. Linux uses something called
a thread group to describe a set of lightweight processes that act as a whole.

