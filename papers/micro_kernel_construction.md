# On micro-Kernel Construction

## Summary
This paper is an analysis of micro kernel design that identifies the key
components of what a micro kernel is and what it needs to provide. It also
addresses performance issues regarding micro kernels.

## Rationale
A microkernel minimizes the core part of a kernel and moving as much
functionality into user space

The reasons for implementing a micro kernel instead of a monolith are
- Clear micro kernel interfaces that enforce a modular system structure
- Servers use mechanisms provided by the micro kernel like any other user
programs, isolating faults just as you would with any other user program
- System is more flexible and tailorable where different strategies for
different uses can coexist on a system

The main concern with micro kernels is the perofrmance. Whether this is because
of the implementation or the fundamental design of micro kernels is unclear.
This paper is a discussion on the fundamental design and functionality
requirements of a micro kernel.

## Some micro-Kernel Concepts
The micro kernel should only implement features that cannot be done in user
space. This can be for the following reasons
- Moving the functionality to user space will break functionality
- Micro kernel needs to support system isolation between untrusted applications

Address Spaces: Physical memory management should be the job of the micro
kernel to allow isolation. The micro kernel should provide primitives for user
space pager and virtual memory manager. The idea is that an address space can
be built recursively starting from the root space (which is essentially
physical address space). The micro kernel provides these primitives
- Grant: Transfer ownership of a page to another address space
- Map: Share pages with another address space
- Flush: Remove shared pages from all other address spaces it is mapped to

I/O: Memory mapped I/O as well I/O done through ports can be implemented by
memory mangers and pagers

Threads and IPC: A thread can be characterized as activity in an address space
that includes registers, stack pointers, state information, and a current
address space that it is a part of. Changes to a thread's address space (moving
it to a different address space i guess) must be controlled by the kernel. This
means that the micro kernel has to implement some idea of threads to prevent
address space corruption. IPC must also be implemented as part of the kernel
because communication between threads must be done with some set of agreements,
these agreements being that the sender can decide the information to send but
the receiver can choose whether to receive that message and how to interpret
it. Micro kernels also have to implement "interrupts". This can be done by
treating hardware devices as special threads that sends interrupts as IPC
messages. The kernel has handle the delivery of this interrupt message but the
device specific semantics is up to the user level device driver.

Unique Identifiers: The kernel must supply unique identifiers to threads or
message channels to make addressing of a source and destination efficient.

## Flexibility
This section lists some kernel functionalities that are usually implemented in
kernel space but can be moved to user space.

Memory manager: The system starts with a MM managing the physical address space
and allows pages to be mapped or granted to user processes. Allows stacked MMs
where a subset of the MMs address space is managed by a different MM

Pager: Works with MM for paging and file mapping. Communication with client and
MM is done purley through IPC. Can create stacked pagers like stacked MMs where
you can combine for different access controls or combining various pagers (one
for each disk) into one composed pager.

Multimedia Resource Allocation: Memory resources may be needed on demand for
real time subsystems. Pagers and MMs can permit fixed allocation of physical
memory for some regions to always be in memory.

Device Driver: Each device has a user space device driver that communicates
with hardware interrupts through IPC messages.

Remote IPC: Dedicated remote communication server that translates local
messages to external communication protocols

Unix Server: Unix system calls implemented by IPC. The UNIX server manages the
memory and paging of its clients

In conclusion, the only things that can't be implemented on top of the micro
kernel are processor architecture, registers, first level caches, and first
level TLBs.

## Performance Factors and Rumors

Kernel User Switches: This section basically says that user to kernel mode
switches overhead isn't an inherent micro kernel issue but more of an
implementation issue. It ocncludes this by looking at two micro kernels, Mach
and L3. Mach has an 800 cycle kernel overhead but mostly due to software
implementation. The L3 kernel is able to do a switch with overhead of between
15 and 72 cycles. A theoertic minimum on kernel mode switches would be 4
instructions on a 1 issue processor

Address Space Switches: Similarly, this section talks about how address space
switching overhead is not an inherent flaw of micro kernels and that it is
possible to them with little overhead. The main overhead of address switching
is with the TLB. Switching the page table base register takes 1-10 cycles but
TLB may have to be flushed and repopulated. With tagged TLBs where you can have
TLB entries from different address spaces at once, the overhead is very small.
With untagged TLBs, flushing is required and reloading working set takes many
cycles. However, it is possible to have efficient implementations of address
switching. The paper provides examples of such implementations that take
advantage of small user address spaces that occupy the same address space as
other user address spaces.

Thread switches and IPC: Compared to UNIX, most micro kernels are at least 2
times faster at thread switching but this can be made even faster. The
conclusion is that IPC can be implemented fast enough to handle hardware
interrupts.

Memory effects: This section goes against another paper's claims that Mach has
higher memory cycle overhad per isntruction than Ultrix (monolithic kernel).
The other paper presented the findings that Mach + Unix server had higher MCPI.
This paper presents that in terms of system vs user cache misses, both systems
had similar user cache misses and that the different was due to system cache
misses. The paper further analyzed this by looking at the types of misses that
were happening in Mach which were conflict and capacity misses. This showed
that conflict to capacity miss ratio was lower in Mach which means that the
MCPI is due to Mach's larger cache sets instead of the inherent micro kernel
design. Basically, the memory overhead is because of Mach implementation.

## Non-Portability
This section discusses the inherent issue of non portability in micro kernels.
Past micro kernels had a hardware dependent layer and an independet layer built
on top of that. This degrades performance because the micro kernel can't take
advantage or avoid specific hardware functionalities or issues. Therefore,
micro kernels should be inherently hardware dependent.

Address space implementation: The key idea in this section was that different
processors have different features and different ways to efficiently implement
page tables. For example, on the Pentium processor, it is better to use segment
registers (registers that hold base addresses of specific segments in memory)
for managing user address space while on the 486 processor, conventional page
table base reigster switching was more efficient because segment register loads
were slow. Micro kernel portability is therefore greatly limited if you want
optimizations based on processors. These optimizations are also needed for
performance.

IPC implementation: The associativity of caches can also affect implementation
of IPC. On Pentium, caches were 2 way associative while it was 4 way for 486.
This meant 486 could have simulatenous accesses without issues but on Pentium,
this would cause conflicts. To address this need to changea align of thread
control blocks which breaks portability

For processors with different architecture, instruction sets, registers,
exception handling, TLB, etc, the micro kernel design itself may need to be
reconsidered. The paper says the following: "micro kernels form the link
between a minimal micro set of abstractions and the bare processor". This means
that micro kernels are really processor abstractions that allow portable
operating systems on top of it. The micro kernel cannot be portable and must be
processor dependent to provide this abstraction correctly.

## Different Micro Kernels
- Synthesis: Runtime kernel code generation but probably won't work on modern
CPU's because of code bloat and cache pollution
- Spin: User supplied code compiled into kernel. Can eliminated IPC but system
call overhead still high.
- Utah-Mach: Modified Mach using migrating RPC
- DP-Mach: Protection domains inside a single address space
- Panda: Minimal kernel with only interrupts and exceptions in kernel with
everything else in user space.
- Cache kernel: Caches only active threads, mappings, and address spaces
isntead of all global state
- Exokernel: Removes all abstractions, exposes raw hardware resources with
secure multiplexing
