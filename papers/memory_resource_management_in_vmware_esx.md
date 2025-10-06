# Memory Resource Management in VMWare ESX Server

## Summary
Traditional servers are consolidated into one machine via Virtual Machines.
VMWare ESX server is designed to efficiently multiplex hardware resources
without having to modify the guest OSes. This paper provides methods for
efficiently memory management for servers.

## Introduction
Servers are often underutilized so multiple servers can be consolidated into
one machines using virtuam machines. With inexpensive shared memory
multiprocessors, there are advantages of consolidation such as multiple VMs
being able to run on one machine without signficant performance penalties. The
core challenges with virtual machines is the need to modify guest OSes to
better cooperate with the hypervisor. THe goal of VMWare is to not have to
modify guest OSes while ensuring performance isolation and resoursce fairness.
The hypervisor must also be able to dynamically overcommit resources to VMs
meaning provide the illusion that more resources than the physical limit
exists. VMWare ESX is different because instead of a hypervisor running on a
host OS (VMWare Workstation), ESX runs on the hardware. ESX virtualizes the
Intel IA-32 architecutre allowing multiple unmodified OSes run on it. This
paper focuses on the memory resource management techniques in ESX
- Ballooning: Coaxing the guest OS to reclaim its own least used pages
- Idle memory tax: Prioritize memory allocation for active VMs
- Content based page sharing: Eliminate redundancy across VMs by transparently
identifying identical memory pages
- Hot I/O remapping: Improve I/O efficieny by remapping frequently used pages

## Memory Virtualization
The goal of memory virtualization is to provide the guest OSes the illusion of
a zero based physical memory space that is identical to physical memory in
hardware. ESX server adds an extra level of address translation between the
guest physical memory and machine memory. Each VM has a pmap data structure
that maps the guest physical page numbers to machine page numbers. This is the
translation layer from guest physical to machine address. VM instructions that
modify page tables or TLBs are interecepted by ESX. Each VM has a shadow page
table that is maintained by ESX and is used by the CPU. This allows translation
to happen in hardware while maintaining coherence with the pmap structure. This
technique allows the ESX to transparently manage memory without the guest OS
being aware of it. This enables techniques such as ballooning, swapping, page
sharing, and I/O remapping.

## Reclamation Mechanism
Memory reclamation is needed because the total memory that the VMs think they
have is much larger than the actual physical memory available. ESX must reclaim
memory when the actual demand exceeds physical availability so that it can
multiplex across many VMs. Traditional approaches involve an extra layer of
paging by the VMM where the VMM needs to make met-level page replacement
decisions that are uninformed and risk double paging where the guest and VMM
both swap the same page. 

Ballooning: This can be overcome by a guest level balloon driver that inflates
or deflates memory usage inside the guest OS on instruction from the ESX
server. Inflating means the driver allocates and pins guest physical memory so
that the guest OS is forced to reclaim pages using its own native memory
management. Deflating means the driver releases the memory so that the OS can
recliam it into its free pool. This allows reusing the guest OS's page
replacement logic and avoids double paging.

Demand paging: When ballooning is not possible or insufficient, ESX pages out
guest memory to ESX controlled swap area on the disk. The swap daemon picks a
page to evict in a way that doesn't conflict with the guest OS algorithms. This
supports asynchronous page outs.

## Sharing Memory
A way to reduce memory consumption is to reduce reundancy between VMs for
similar or identical OSes, applications, or datasets. VMs duplicate many of the
same memory pages (shared libraries, kernel code, zero pages). Prior research
such as Disco showed that page sharing is an efficetive way to reduce this
redundancy but this requires guest OS modifications.

Content Based Page Sharing: ESX overcomes the need to modify guest OSes by
comparing the page contents instead of tracking memory semantics. ESX scans
guest pages periodically and computes hashes of page content. A global hash
table contains hashes of memory content and if a match is found, a byte by byte
comparison is done to compare the content. If there is a match, both pages get
mapped to the same machine page and is marked as copy on write. If there isn't
a match, pages are marked as "hint" entry which means future matches may reuse
it. The pages are scanned randomly to balance overhead except for when pages
are being swapped out where it is scanned to see if it can be shared.

Tests with CBPS showed that with Linux VMs, up to 67% of memory was shared,
mostly coming from redundant code and data. Tests with real world deployments
showed a range of 10 to 43% of memory being shared and 7 to 33% being
reclaimed. This approach has negligible CPU overhead.

## Shares vs Working Sets
Memory allocation has two conflicting goals. Perfomrance isolation using
proportional share fairness and efficient memory utilization by reallocating
unused or idle memory. VMs with high shares but low usage can hoard memory
while low share high usage VMs can suffer under memory pressure. Proportional
share framework allocates each VM with a number of shares representing their
entitlement to memory. A VM receives memory in proportion to its share count
which guarantees minimum memory allocation under contention. When memory is
needed, it is claimed from the VM with the lowest price per page. However, the
problem with this approach is that pure share based systems ignore actual
memory usage. Idle VMs with lots of shares can hoard memory while active VMs
with little shares suffer. The authors introduce a tax on idle memory pages
meaning they cost more to the VM. Reclamation prefers idel pages. The method
for measuring idle memory doesn't involve guest OSs because each OS has
different memory tracking and is focused on process level memory usage not VM
wide. ESX uses a statistical sampling approach where it randomly samples 100
pages every 30 seconds of guest execution and invalidates the sampled page's
TLB entry and page mapping. If the guest OS accesses the page, it is considered
touch and the estimated working set is the number of pages touched divided but
the number of samples. It also uses moving averages (slow, fast, and
insantaneous) and takes the maximum of these three as the final estimate of the
working set. Experiments show that sampling tracks actual usage closely. Idle
memory tax effectiveness was measured with two VMs where one was idel and one
was running a workload. Taxation showed 30% throughput increase for the active
VM.

## Allocation Policies
This section details how the authors coordinate multiple techniques to form a
unified policy driven framework that adapts dynamically to system load and user
defined constraints. Each VM is assigned three memory parameters, min size, max
size, and shares. Before a VM is powered on an ESX server, it must reserve the
min + overhead amount of memory in RAM and max - min amount of swap. ESSX
dynamically rebalances memory when VM memory parameters are changed, VMs are
powered off, memory usage patterns shift, or there is more free memory than a
predefined threshold. Different memory pressure states dictate how reclaiming
is done. With high memory pressure, there is no reclaiming, with soft
ballooning is used, with hard memory is paged and with low memory pressure VMs
with allocations above their target are paused. The authors test the dyanmic
reallocation with 5 windows Vms running variouse applications. Each VM is given
1.47 GB when there is only 1GB available. When VMs boot, memory spikes
triggering paging. Zero pages can be shared across VMs so ESX can reclaim some
memory. Once application activity bgeins, sharing drops and ballooning takes
over to reclaim memory as needed.

## I/O Page Remapping
I/O performance can be improved by eliminating unnecessary memory copies caused
by hardware addressing limitations. Many I/O devices can only DMA to the lower
4GB of memory. This causes problems because on x86 systems, physical address
extension supports up to 64GB of physical memory and guest pages may be located
in memory regions where the devices can't DMA. Traditional workarounds involve
the OS copying the data from high memory to low memory before initiating DMA.
ESX instead transparently remaps guest physical pages from high to low memory
without the guest OS knowing. "Hot" pages involved in repeated I/O are tracked
using a software cache of guest physical to machine page mapping and the number
of times a page is copied for I/O. After a usage threshold, it is remapped to
lower memory region to avoid further copying.
