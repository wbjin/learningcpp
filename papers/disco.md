# Disco: Running Commodity Operating Systems on Scalable Multiprocessors

## Summary
Disco is a virtual machine monitor that sits in between the hardware and OSes
to allow multiple VMs to run on a single multiprocessor machine. It absracts
away hardware specific details such as NUMA to allow commercial OSes to run
simultaneously and fully utilize machine resources.

## Introduction
The paper says that there is a big gap between hardware capability and OS being
able to fully utilize this capability. To run modern OSes on multiprocessor
systems, the OS needs to be extensively modified which is takes a lot of work.
The paper presents a solution to this by creating a layer between hardware and
the OS that allows multiple OSes to run on a single machine. This reduces the
complexity needed as you don't need to heavily modify the OS. Disco is the
prototype implementation of this concept. The paper presents its findings that
the cost of virtualization is low and 8 VMs can run workloads faster than
standard OSes.

## Problem Description
The paper says that because hardware innovations require big changes to OSes,
the system software lags behind hardware leading to late and unstable OSes.
This undermines hardware innovation because there is no software that can
eliably run on this new hardware. For example, shared memory multiprocessor
systems have different access times to memory because of the difference in
proximity. The OS has to change its memory access patterns to accomodate this.

## A Return to Virtual Machine Monitors
Instead of changing the OS to adapt to the hardware, VMM can virtualize
hardware resources and provide conventional hardware interface to OS. Multiple
VMs can run on one ccNUMA multiprocessor using this. Each VM is encapsulated
and acts as scalable, fault contained units that can efficiently share
resources with each other. The idea is to simulate a cluster like environment
on one multiprocessor. This makes it so that commodity OSes don't have to worry
about things like NUMA. 

While this approach has many benefits, there are also costs, specifically the
additional overhead to run a VMM and multiple OSes.
- Overhead: Need to emulate privileged instructions and need extrace exception
handling. Need to virtualize the I/O. OS code has to be replicated for multiple
VMs. Redundant file system structures across VMs
- Resource management: The VMM doesn't have the OS level knowledge such as not
being able to distinguish between idle loops and useful compute. Need to make
policy decisions without the context OSes have
- Sharing and communication: No easy way to share files or devices. Behave like
stand along systems even though hardware is shared.

The paper presents solutions and ways to overcome some of these limitations.

## Disco: A Vritual Machine Monitor
Disco is designed for a cache coherent non uniform memory access multiprocessor
called FLASH. It has multiple nodes each containing a processor, memory, and
I/O devices. The primary function of Disco is to virtualize hardware resources
by exporting simplified hardware interfaces that OSes call. 

Disco Interfaces: Disco has interfaes for processors, memory, and I/O devices.
- Processor: Emulates MIPS processor with all instructions, MMU, and traps.
Frequent special instructions like privilege switch was mapped to load and
store on predefined addresses.
- Memory: Presents contiguous physical memory. Handle's FLASH's NUMA with page
migration and replication
- I/O: Each VM has virtual I/O devices. Disco intercepts and emulates I/O
communication. Important I/O devices had special abstractions. Virtual Disks
had private and shared access modes that could be configured for persistent or
non persistent data. Virtual network interfaces were implemented as subnets
withing Disco. Disco acted as a gateway to outside network

Disco Implementation: The key implementation details of Disco lies in the share
memory, multi threaded architecture, NUMA optimizations, cache aware and wait
free datastructures, and efficient IPC. There was also a focus on small
codebase for easier tuning.
- Virtual CPUs: Virtual CPUs run on physical CPUs to give VMs the illusion that
it is the only one on the processor(s). Registers and PC are stored as vCPU
state and most instructions run at native hardware speed. For privileged
instructions such as TLB changes, Disco detects and emulates the instruction in
its software. Disco runs in kernel mode with full hardware access. When running
the guest OS, processor runs in supervisor mode for guest kernel and user mode
for guest user application. Supervisor mode gives access to protected memory
regions but not privileged instructions. The processor traps into Disco which
emulates the trap on the vCPU by redirecting to the guest OS trap handler
table. vCPU is time shared scheduled across the physical CPUs. vCPUs are
scheduled on CPUs close to their memory for NUMA locality.
- Virtual Physical Memory: Each VM sees their physical memory starting from
address 0. This virtual physical address is mapped to machine addresses. The
software TLB of MIPS is used for translations. When guest OS inserts TLB entry,
Disco intercepts it and inserts the correct physical address. Each VM has pmap
table that has pre computed TLB entries and backmaps to track virtual addresses
and invalidate entries if pages are reclaimed. MIPS allows kernel mode to
access physical address directly but this breaks Disco abstraction. The OS code
was relinked into a mapped region. MIPS has a tagged TLB to avoid flushes but
Disco simply flushes TLB every switch to simplify things. There are more TLB
misses because of this flush. To overcome this, Disco added second software
level TLB cache. 
- NUMA Memory Management: Disco actively manages memory on ccNUMA systems by
migrating or replicating pages. Remote memory access is slower than local
memory access so Disco migrates and replicates pages to be closer to the
process running on that node. FLASH has hardware cache miss tracking which
Disco can use to detect hot pages. When migrating a page, Disco invalidates TLB
entries, copies the page to local memory, and updates the mappings to the new
page. When replicating, it downgrades existing mappings to read only, copies
the page to local memory, and updates TLB so multiple nodes can access.
- Virtual I/O: Device accesses are intercepted by Disco and instead of
emulating hardware devices, Disco uses special device drivers in the guest OS
that issues a monitor call with all command arguments at once. For DMA
handling, the Guest OS specifies a physical address in the DMA request but this
is translated to machine address by Disco. If a device is only being use by one
VM, that VM has exclusive access. Disco uses DMA control for cross VM sharing
like copy on write disks where multiple VMs share the same disk image and
changes are stored separately. 
- Copy on write disks: If a requested disk block is already in memory, Disco is
directly mapped into VMs physical memory and writes trigger copy on write.
Multiple VMs can share memory pages this way for read only. Writes are kept
private to the issuing VM. Two B-Trees are used for this, one global disk cache
B-Tree that maps the disk sectors to machine memory addresses and Per VM
modification B-Tree that tracks each VMs changes to disk blocks. This sharing
only applies to non persistent disks that use COW for read only sharing.
Persistent disks are mounted by one VM at a time to avoid conflicts and other
VMs us protocols like  NFS.
- Virtual Network Devices: VMs use protocols like NFS for communication which
would cause duplicate data as it is. Disco creates virtual subnets that allow
direct inter VM communication and avoid redundant memory copies. Each VM has an
Ethernet like virtual NIC that supports scatter and gatther buffers. Pages are
remapped instead of being copied. Data transfers between VMs use DMA and COW
mapping. Data sent between VMs is mapped as read only. File caches are also
shared bewteen VMs. 

Commodity OSes: Disco is OS-independent meaning it works for varous different
OSes. Modern OSes already have a hardware abstraction layer that the changes
would go into. Specific changes such as MIPS bypassing the TLB conflicts with
Disco's virtual memory model so the kernel code and data was mapped into
supervisor space. Disco uses monitor call interface for OS device drivers to
interact with Disco. Other systems like HAL had specific optimizations like
making traps special load and store instructions on a mapped page. Disco can
also run application specific OSes like Splash OS.

## Experimental Results
Experiments were conduected on SimOS simulator since FLASH was not yet
available. Simulated processor with statically scheduled, high clock rate CPUs
with simulated memory access latencies for NUMA. SimOS allowed running
realistic but short workloads while allowing authors to study CPU/memory
overheads. 

Execution Overhead: The overhead was compared with running directly on IRIX
versus IRIX on Disco. Overheads were 3-16% depending on the workload but mostly
due to trap emulation of TLB leading to high miss rates. A workload running
pmake was the most stressful due to its heavy use of file system. Overheads
were mainly from privileged instructions that have to be emulated. 

Memory Overhead: To test memory overhead, 8 concurrent pmake each using
separate disk was run on a machine with 8 processors. Different configurations
were test like running an OS on IRIX, running 1, 2, 4, and 7 VMs. To measure
memory footprint, virtual footprint (memory needed without sharing) and machine
footprint (the actual memory used with Disco) was captured. The experiment
showed that kernel buffer cache allowed sharing across VMs limiting memory
growth with more VMs. Some things can't be shared like kernel data but that is
still modest with less than 20MB with 8 VMs. This shows that multiple VMs don't
significantly increase memory overhead.

Scalability: To test scalability, 8 instances of pmake with different numebrs
of VMs on Disco versus running on IRIX direclty were compared. IRIX by itself
is not NUMA aware creating hotspots in kernel data structures and memory stalls
due to remote misses. A single VM on Disco caused extra idel time and slower
critical sections resulting in higher overhead than bare IRIX for pmake.
Splitting the workload into 8 VMs caused execution time to drop 60% of just
IRIX where gains were mainly from reduced kernel stall time.

Dynamic Page Migration and Replication: Disco dynamically migrates and
replicates memory pages for data locality on NUMA hardware. Two workloads were
run, 6 Verilog + 6 memory system simulations on 8 processors in the same VM and
raytrate running on 16 processors. Each workload ran on IRIX on NUMA, Irix on
Disco, IRIX on UMA. Both workloads were faster (33% and 38%) on Disco.
Improvements came from remote stall times. Comparison with UMA hardware showed
that Disco can't completely hide NUMA effects but can narrow the gap. 
