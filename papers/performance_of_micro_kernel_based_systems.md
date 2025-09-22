# The Performance of Micro Kernel Based Systems

## Summary
This paper is a study on the L4 micro kernel and whether or not it overcomes
the limitations of previous micro kernels like Mach that had not very good
performance. The paper runs benchmarks done previously on older micro kernels
as well as doing benchmarks with a ported Linux kernel on top of the L4 micro
kernel to show that L4 is performant and that it is possible to have a high
performing conventional OS on top of a micro kernel.

## Introduction
OS research has largely abandoned micro kernels at the time of this paper due
to its performance concerns. Common critiques of micro kernels are that the
abstractinos are either too low meaning the micro kernel doesn't provide enough
abstractions and there aren't enough mechanisms or too high meaning the
abstrations are too closely tied to the hardware and is too rigid. This paper
reviews the L4 second generation micro kernel and evaluates its performance on
same benchmarks as past papers but also new benchmarks with a ported Linux
kernel that runs on top of L4. 

## L4 Essentials
The L4 micro kernel implements the minimum required functionalities of a micro
kernel which are threads, address spaces, and IPC. The definitions of these are
very similar to the one described in the Micro Kernel Construction paper. The
key idea is that the initial address space (physical memory) is used to create
recursive address spaces managed by user level pagers and memory managers
through the grant, map, and flush primitives. I/O and interrupts are also as
described in the Micro Kernel Construction paper where I/O ports are treated as
memory opjects and hardware interrupts are delivered as IPC messages from
special hardware threads to device drivers that are running in user level.
There are some hardware specific optimizations such as on the Pentimum
processor which has the segment registers that allows for multiple small
address spaces to belong in the same page table saving costs on context
switching.

## Linux on Top of L4
Linux Essentials: Most of Linux is architecture independent (process
management, resource management, file systems, newtorking, device drivers)
which makes it easier to port just the architecture dependent parts. The parts
that do need changing are interrupt handling, context switching,
copyin/copyout, system calls, and memory management mapping. Linux uses a three
level page table that is architecture independent. The architecture dependent
part has to map this abstraction to the hardware page tables and the software
TLB handlers. Linux interrupts are separated as top halves or bottom halves.
Top halves are trigerred directly by the hardware and can interrupt each other
and the bottom half interrupts. Bottom half interrupts are scheduled by the top
half interrupts and cannot interrupt each other or the top half.

## L4 Linux
L4 Linux is fully binary copatible with Linux on x86 so that any off the shelf
Linux would run on L4 Linux. Therefore, the only changes to Linux were
architecture depenent parts. The L4 micro kernel also had no Linux specific
changes to test the portability of the micro kernel and whether its interface
was flexible.

Linux Server (Linux Kernel): The kernel essentially runs as a single task on
top of the micro kernel. In native Linux, the kernel address space is mapped
directly on physical memory. In L4 Linux, the task requets for a subset of the
physical memory from the L4 pager and this is mapped one to one with L4 Linux
Kernel's virtual address space. Then, the L4 Linux task acts as a pager for all
the user processes it creates. However, because user level tasks don't have
access to hardware page tables, Linux server has to maintain a separate logical
page table which means memory consumption due to page tables double. The Linux
Server is also just a single thread that multiplexes system calls to avoid
blocking. 

Interrupt Handling: Linux top half interrupts are implemented as threads
waiting for IPC messages. Bottom half threads are executed after the top half
thread has been completed.

Linux User Processes: A user process is a L4 task that is mapped into the Linux
server address space. The Linux server acts as the pager for this user process.
A small emulation library is mapped into the address space of each user process
to package and forward the system calls to the Linux server. None of the
emulation actually implements the system call.

System Calls: System calls are interecepted in three ways: 1) a modified
libc.so object that applications can dynamically link with and uses L4 IPC
primitives to call the Linux Server. 2) libc.a for statically linking with a
modified libc. 3) An execption handler that emulates syscall traps by
redirecting to the modified library. The Linux server also runs in a small
addresss space which allows improved address space switching with simulated
taggeed TLB. This improves IPC performance. Native Linux maps the current user
address space into the kernel address space and copies data bewteen user and
kernel with the address translation being done by the hardware. On L4 Linux,
this had bad performance so the server uses physical copyin and copyout with
the server doing its own logical address translation.

Signaling: In native Linux, signals are delivered by directly modifying the
user process's stack, stack pointer, and instruction pointer to deliver a
signal. This is no allowed in L4 so each user process has a dedicated singal
handler thread in the user's address space and does the modification for the
interrupt sender

Scheduling: The L4 micro kernel handles the scheduling of threads which means
the Linux server doesn't have to control when the threads run. The Linux server
uses coroutines to multiplex system calls so that concurrent calls don't block.
The micro kernel scheduler has hard priorities with a round robin scheduling
per priority level. 

Support Tagged TLBs or Small Spaces: TLB flushes are expensive so processors
increasingly support tagged TLBs that allow multiple address spaces to exist in
the TLB using process IDs. Those without tagged TLBs can use the small address
space workaround with segment registers allowing multiple user address spaces
to be part of one address space.

Dual Address Space: This section mentions a mistake in the initial
implementation of the Linux server. Initially, to stay true to native Linux,
L4Linux tried to map the kernel address space to each user address space.
However, the Linux server had limited virtual address space to hold multiple
user address spaces so L4 had t replicate the server address space per process.
This meant multiple server threads which introduced complexity and performance
loss. This also made the small address space optimization not possible and
needed a TLB flush every switch. Instead, switched to one address space per
user process and using software based address translations between kernel and
user instead of hardware based ones.

## Compatibility Performance

This section performs benchmarks on L4Linux running on L4 versus a different
Linux port called MkLinux that runs on Mach. The same hardware was used to run
the measurements on both systems

Microbenchmarks: `getpid` was used to measure the system call overhead. L4Linux
needs 300 more cycles than native Linux while MkLinux in kernel mode needs 3.9
times that of L4 and user mode needs 29 times more. Comparing the overheads in
native Linux and L4Linux, the authors say that it is mostly due ot the IPC
needed for system calls.

Macrobenchmarks: One test was the time needed to compile the Linux server where
L4Linux was 6-7% slower than native Linux. A multiuser systems test was used to
determine the max throughput of a system. On native Linux, this was around 130
jobs per minute, on L4Linux 123 jobs per minute. On average, L4Linux is 6.8%
slower than native Linux. 

The conclusion is that the L4Linux implementation comes reasonably close to
native Linux performance. The microbenchmarks show that the performance of the
underlying micro kernel matters as shown with the comparison with MkLinux. It
also shows that co-location, moving the Linux server to kernel mode in the
micro kernel, is not enough to overcome performance deficiencies.

## Extensibility Performance
The primary benefits of a micro kernel is the extensibility that it is supposed
to provide. This section asks whether or not this is truly possible.

Pipes and RPC: This section discusses replacing Unix's traditional pipes with
custom IPc mechanisms. The idea is that pipes can be emulated with the
efficient IPC mechanisms in L4. The findings show that L4 based pipes
outperform native linux pipes. This is possible due to the synchrnous messaging
that is possible with IPC and memory mapping without the kernel needing to copy
things.

Memory operations: This section evaluates how efficient L4 handle memory
management compare to native Linux. L4 allows page faults to be handled by user
processes with a custom pager. Benchmarks shows that L4 has faster trap
handling and working with protected pages. L4's extensible memory management
allows better performance due to customized paging, fast IPC, and light weight
execption delivery with minimal kernel intervention.

Cache Partitioning: User level paging and cache partitioning improves the worst
case time complexity. Hardware caches can be unpredictable with multiple
threads and context switches and traditional Linux can't isolate cache usage.
L4 allows hierarchical user level pagers which means multiple paging systems in
parallel and custom pagers that can manage memory with awareness of the cache
layout. Cache partitioning reduces cache interference and for real time
applications this can mean 4 times the performance.

## Alternative Basic Concepts

Protected Control Transfers: A lower level parameterless cross address space
call. This can be considered as an alternative to IPCs to improve performance
but it seems like the paper concludes that IPCs are enough.

Grafting: Downloading extensions into the kernel. The idea is that grafting
could reduce overhead from user to kernel transitions. However, IPC has better
security properties.
