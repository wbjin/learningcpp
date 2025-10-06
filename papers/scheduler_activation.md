# Scheduler Activations: Effective Kernel Support for the User-Level Management of Parallelism

## Summary
This paper is about threading and how to schedule threads effectively.
Threading is essential for parallelism and there are two models, kernel threads
that are well integrated with the kernel but is slow and user level threads
that are fast and flexible but has bad integration with the OS. The authors
present a new kernel interface and a user level thread package that combines
these two.

## Introduction
There are two traditional thread models.
- User level threads: Fast and flexible because it avoids kernel traps. However
has poor integration with the kernel because the kernel is unaware of user
level threads. User threads are managed entirely in user space and has cheap
context switching overhead with customs cheduling policies. However, a single
blocking user level thread can block all other threads that are blocking. It
can also be preempted without user space knowing. User level threads run on top
of kernel threads where multiple user level threads can be mapped to on kernel
thread allowing the user level threads to have different logical stream of
executions
- Kernel threads: Integrated with the OS so has full capabilities like I/O,
preemption but is slower due to higher overhead. It is managed by the OS and
has high context switching cost. Because the kernel is aware of this thread, it
can be scheduled off when blocking allowing another kernel thread to run.

The goal of this paper is to introduce a hybrid mechanism that preserves user
level performance and flexibility with kernel level functionality. It does this
by giving each application a virtual multiprocessor abstraction where the
kernel manages processor allocation across different applications. The
application in turn manages how threads are run on the multiprocessor.

## User-Level Threads: Performance Advantages and Functinoality Limitations
User level threads are performant and flexible. It does not require system
calls to switch which allows it to not trap into the kernel. It can also be
tailored to each applications needs which avoids the generality that kernel
threads need to provide. This means that user threads can have custom
scheduling while kernel threads are subject to the scheduling policy of the
kernel. However, functionally, it is not complete because of the lack of
integration with the kernel. For example, if one user level thread initiates a
blocking call, the user thread manager isn't aware of blocking meaning it can't
schedule other threads. Or when a user level thread faults, the kernel suspends
the thread silently without the thread manager knowing. Additionally, the
kernel scheduler is oblivious to user threads so it may schedule low priority
threads or run a kernel thread with no user threads to execute. The idea that
this paper proposes is that running user threads on top of kernel threads is
the wrong abstraction

## Effective Kernel Support for the User-Level Management of Parallelism
This sectiond describes the scheduler activations abstraction that combines
user level and kernel level threads. The goal is to combine the performance of
user level threads with the completeness of kernel threads. To allow this, the
kernel gives each user threading system a virtual multiprocessor abstraction.
In this abstraction, the kernel is responsible for assigning applications a
real processor and notifying it of processor changes, I/O, preemption, etc. The
user level thread manager is responsible responding to kernel events and
scheduling threads onto assigned processors. The kernel and user thread
managers communicate with scheduler activations. The kernel notifies the user
manager about processor preemption, blocking, page faults, processor allocation
changes, etc and the user level scheduler can act on this notification. The
user manager notifies the kernel when it needs more processors or if some
processors are idle. A scheduler activation is a kernel provided execution
context. For each processor that the application is allocated, there is an
active scheduler activation context. It acts like a kernel thread and it is
created when there is an event that the kernel needs to notify the user manager
of. Once the user level manager is notified, only it is allowed to decide how
to schedule this scheduler activation context. Scheduler activation contexts
are reused (kind of like a thread pool) instead of creating a new one for every
event.

## Implementation
The authors implemented this system using the Topaz kernel and a user level
threading library called FastThreads. The authors changed the kernel thread
system in Topaz to make upcalls into user level threads instead of directly
resuming threads. FastThreads was changed to handle upcalls and to resume
preempted critical sections. The changes made by the authors provided
mechanisms for application managed parallelism. It didn't introduce any policy
decisions. Processor allocation was done dynamically based on existing work.
The idea was that multiple applications could run on multiple processors to
ensure no processor is idle if there is work to run. The implementation also
allowed kernel threads to run simulatenously with scheduler activations. At the
user level thread manager, each processor had a ready list that was used to
maintain threads on the same processor for cache locality. If there wasn't any
threads, it stole from other lists. To handle cases where preemptions happened
during a critical section, the authors employed a code copying technique where
the original code was copied with yields back to the upcall being added. If a
thread was preempted in a critical section, the copied version would run
allowing upcalls to the user level manager.

## Performance
The authors evaluate scheduler activations on thread operation costs, upcall
overhead, and real world application performance.
- User level thread operation cost: Operations like null fork and signal and
wait showed that were only microseconds of degradation. 
- Upcall performance: Notifying user level managers of events was measured with
singal-wait scheduler activations where it was observed to be 5x slower than
Topaz kernel thread equivalents. The authors conclude that upcall performance
can be improved with hand tuning and writing in assembly.
- Application performance: Authors used a N-body simulation and a classic
parallel algorithm. The authors first tested the speedup by increasing the
number of processors. Kernel threads bottlenecked while fast threads and
scheduler activations both had good speedups. The second case introduced
limited memory and I/O induced blocking which forced paging and blocking of
user threads. Topaz and FastThreads decraded performance rapidly while
scheduler activations maintained performance. As I/O latency was increased, the
performance gap also grew. The last test was simply two parallel jobs. This
showed that shceduler activations beat both kernel threads and user level
threads.
