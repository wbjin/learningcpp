# Shenango: Achieving High CPU Efficiency for Latency-sensitive Datacenter Workloads

## Summary
Datacenter workloads demand microsecond tail latencies and high request
throughput from the operating system. These workloads also have high variance
where load varies across multiple timescales. To achieve microsceond-scale
latencies, kernel-bypass technology such as spin polling the network card in
user space has been developed. However, this approach wastes CPU as you must
dedicate enough cores for peak expected load. Shenango solves this by
reallocating cores across applications at very fine granularity, every 5
micros.

## Introduction
Datacenter applications require microsecond-scale tail latencies and high
request rates, especially for services with service times of around
microseconds (memcached, RAMCloud). Networking hardware improvements make it
possible for microsecond RTTs but the current operating system acts as a
bottleneck, Furthermore, to achieve CPU efficiency, datacenters colocate batch
processing applications and latency sensitive real time services so that the
batch processing application can use unutilized CPU cores during low traffic.
To achieve microsecond latencies, kernel bypass techinques have emerged to spin
poll the NIC. However, this requires allocating dedicated CPU cores for polling
and reduces CPU efficiency as more time is spent spin polling instead of doing
meaningful work. These kernel bypass techniques lack reallocation methods for
CPU cores meaning it must be provisioned with enough cores to handle peak load.
This is exacerbated by the fact that datacenter workloads have bursty arrival
patterns at timescales of microseconds. This means that there needs be extra
idle cores to handle these bursty workloads. Existing OSes are not fit to
handle microsecond scale I/O because it is tuned for millisecond-scale.
Scheduling decisions in the Linux kernel are mde every four milliseconds and
50-100 in Arachne and IX. Shenango is a system designed to achieve
microsecond-scale tail latencies while maintaining high CPU efficiency while
keeping the existing programming abstractions such as synchronous I/O and TCP
network sockets. Shenango makes core allocation decisions every 5 microseconds
with an efficient algorithm that detects with applications would benefit from
more cores. 

## The Case Against Slow Core Allocators
To illustrate why millisecond scale core allocation is not sufficient, the
authors built a simulator that determines upper bounds on the CPU efficiency of
a core allocater that adjusts at one millisecond intervals. The simulator
models a FCFS queuing system and determines the the minimum number of cores
needed to maintain a tail latency limit. This simulator shows that CPU
efficiency remains low with this core allocation interval because it had to
pre-assign more cores than needed most of the time since it could not react
fast enough to the chaning load. This creates a sawtooth pattern where
efficiency drops whenever a new core is added. This new core is provisioned
before the actual workload requires another core so it remains idle. Shenango
avoids this by reallocating every 5 microseconds which allows instant reaction
and avoiding the idle time.

## Challenges and Approach
Shenango's goal is minimize a term coined by the paper called compute
congestion. Compute congestion is when an application's work is delayed because
it didn't have the cores it needed. While it is impossible to make core
allocations per request, Shenango can do better than existing solutions by
doing core allocations every 5 microseconds. However, core allocations come
with challenges of their own
- Overhead: The core allocation interval is limited by the overhead that comes
from determinint that a core should be reallocated, instructing an application
to yield a core, and reallocating that core to a different application.
- Estimating required cores: Estimating the number of cores an application
requires is difficult. Previous sstems used application metrics such as
latency, throughput, or core utilization but this isn't applicable at
microsecond-scale.

Shenango addresses these challenges with an efficient congestion detection
algorithm that uses thread and packet queuing delays as signals. To inspect an
application's thread and packet queues, Shenanog requires a centralized
component called the IOKernel. IOKernel runs as a process with root privileges
that busy spins on the NIC hardware queue. IOKernel can inspect the thread and
packet queues allowing it to orchestrate core allocations. It also does the job
of steering packets to the correct core that is polling. This allows core
allocations to run in 5.9 microseconds with less than two microseconds being
spent on the IOKernel. Commnication between the IOKernel and the application
happens through a per-application runtime that communicates with the IOKernel
with shared memory. These runtimes are user space libraries that provide things
like threads, mutexes, condition variables, and network sockets. At start-up,
the runtime creates multiple kernel threads each with a local runqueue where
the size is the maximum number of cores the runtime is allowed to use. User
applications are run as lightweight user-level threads that are placed in these
runqueues (each core has a kernel thread and a runqueue associated with it).
Shenango can run in an unmodified Linux environment and can coexist with the
Linux scheduler.

## IOKernel
The IOKernel is the heart of Shenango and does mainly two things.

1. Decides how many and which cores to allocate to an application.
2. Handles all network I/O by bypassing the kernel. For receives, it polls the
   NIC queue and multiplexes it to the correct runtime. For transmits, it polls
the runtme egress queues and forwards it to the NIC packet queue.

The IOKernel makes core allocation decisions in two steps 1) Whether an
application needs more cores. 2) Which core to give it (which to take it from
potentially).

Each application is guaranteed a number of cores and a set of bursty cores. A
runtime is entitled to all of its guaranteed cores without preemption and if
needed, IOKernel can assign it more bursty cores to accomodate bursty traffic.
The IOKernel's goal is to assign each application the minimum number of cores
needed while avoiding compute congestion. Application's can yield cores through
each core's dedicated kernel thread. When the kernel thread's runqueue is empty
and there is no work that can be stolen from the other runqueues, it will yield
its core to the IOKernel.

The IOKernel detects compute congestion based on two signals, a runtime's
queued threads and queued ingress packets. If any item is found in the queue
for two consecutive runs of the algorithm, it indiates compute congestion and
that the application could benefit from more cores. Detection of these items
can be made efficient with the use of ring buffers. You can compare the current
head pointer to the last iterations tail pointer. If the head pointer of the
current ring buffer is still greater than the head pointer of the previous
iteration (the runqueue was not consumed fast enough), then it means that an
item was in the queue for two runs of the algorithm.

The IOKerneld decides which core to allocate to an application based on three
factors
1. Hyper-thread efficiency: Hyper-threading allows two hardware threads to run
on the same core. This allows them to share caches. Hyperthreads from
different applications will result in cache contention and degradation of
performance for both threads.
2. Cache locality: Giving an application a core it ran on before can result in
less cache misses. Furthermore, with hyperthreads, if an application's
thread is already running on the core, assigning the same application that core
will result in good cache locality.
3. Latency: IOKernel always prioritizes allocating from idle cores instead of
preempting a busy core.

The IOKernel's algorithm for detecting if a core can be allocated goes as follows:
1. If core is idle, can be allocated to app.
2. If there are no idle cores and the application currently using this core is
using more than its guaranateed number of cores, can be allocated to app.
3. Otherwise, cannot be allocated.

The IOKernel's algorithm for actually selecting a core to be allocated goes as
follows.
1. For each core the application is currently using, if the core can allocate
accomodate another hyperthread, use that core
2. If the core most recently yielded by an application is allocatable, use that
core
3. If there are any idle cores, use the idle core
4. Else, randomly choose a core from a bursting application

Once a core is chosen, the IOKernel tries to assign it a kthread. It tries to
use the kthread that most recently ran on that core, and if that kthread is not
available, it uses the kthread that had been sitting idle for the longest.

The IOKernel also handles network I/O by busy looping on the NIC packet queue
and application packet queue. It forwards the incoming packets in the NIC queue
to the correct runtime. Each runtime is assigned an unique IP address and MAC
address. The IOKernel checks which runtime the packet is intended for and
directs it to the core that the runtime is running on. The IOKernel also polls
the egress packet queues of active kthreads.

## Runtime
The Shenango runtime is optimized for compatibility with existing facilities.
It provides scalable user threads, synchronous I/O operations, and
compatibility with BSD sockets. Applications are linked with the runtime and is
able to still interface with the Linux kernel for memory allocation.
Applications are discouraged against using kernel syscalls, instead the runtime
provides kernel bypass alternatives to these syscalls.

The runtime is also responsible for scheduling across the cores that an
application is allocated. Upon startup, it registers its kthreads (equal to the
number of max cores) and establishes a shared memory region for packet queues.
Each kthread maintains a runqueue and is able to steal work from runqueues of
other kthreads of the application. The runtime also has optimizations such as
lock free user thread wakeups and fine grained work stealing of user threads
inspired by other runtimes like Go and ZygOS. The runtime also allows
run-to-completion allowing user threads to run until completion, improving tail
latency. 
