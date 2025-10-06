# Practical, transparent operating system support for superpages

## Summary
As memory size increases, TLB can't keep up leading to more TLB misses which
degrades performance. Superpages are large pages that can improve TLB coverage.
However it is hard because of challenges with fragmentation, allocation,
promotion, and eviction. This paper presents how to handle such challenges with
superpages.

## Introduction
TLB is used to speed up virtual to physical address translation. However, even
though main memory size has increased, TLB size has remained constant which
leads to a larger working set and more frequent TLB misses. Superpages are a
solution to this where large memory pages can be allocated to reduce pressure
on TLB. However, there are various challenges with superpages such as increased
memory footprint, more paging I/O, fragmentation of memory. Existing OSes
provide little support for superpages. This paper presents a general and
transparent superpage management system that reserves large contiguous memory
regions in advance, creates superpages incrementally as pages are accessed, and
reclaims unused reservations and restores contiguity when needed.

## The super page problem
While main memory size has been increasing, TLB size has stayed constant (1MB)
due to latency constratints with typically only 128 entries. Modern
applications typically exceed this coverage and TLB misses are more costly now
due to on chip caches being larger than TLBs and cache hits still being slow
due to TLB misses. Superpage allow TLB to cover more memory with fewer entries
but is challenging for various reasons. Superpages are constrained by certain
hardware characteristics such as limited set of page sizes, the need for
alignment to page size and contiguity (superpages need to be physically
contiguous), and limited status bits for a large page making protection coarse.
There are various tradeoffs involved in the management of superpages
- Allocation: Pages can be allocated with any available page frame or use a
reservation based strategy where a contiguous region is reserved in advance in
anticipation of more accesses. This reduces need for page allocations later but
you can end up overcommiting leading to internal fragmentation
- Fragmentation control: Multiple page sizes can cause memory fragmentation
where the OS must preempt unused reservations or reclaim inactive memory
- Promotion: Base pages can be promoted to super pages when a reservation is
fully allocated. Promoting early can save TLB misses but increases memory
overhead if unused pages are used for the promotion
- Demotion: Superpages can be broke into regular pages under memory pressure
but hardware only tracks usage by superpage so the OS can't know which base
pages in the superpage is active
- Eviction: Superpages need to be evicted but there is only one dirty bit for
the entire superpage so even if one base page is dirty, entire superpage has to
be written

## Related approaches
This section covers existing methods for superpage support

Reservation based approaches: When there is a pagefault, the OS reserves a
contiguous region of memory. When enough pages within the reservation is used,
a superpage is created. Existing solutions had ways to preempt reservations
under memory pressure or using eager promotion. The drawbacks for these methods
were that it required manual tuning via user-specified hints. If bad hints were
provided, it could lead to poor TLB coverage and wasted memory and I/O

Page relocation approaches: After allocation, the OS can relocate pages to make
them contiguous for superpages. Implementations of this used cost benefit
analysis to decide whether to relocate and promote to a superpage. This
requires a software managed TLB which can track usage via the TLB miss handler.
The drawbacks of this were that there were more TLB misses before the promotion
and these misses were more expensive because of a more complex TLB miss
handling. The authors say that reservation and relocation can be used together
where relocation only happens when reservation fails. This allows background
compaction to improve contiguity

Hardware suport approaches: The hardware can also be modified so that pages
don't have to be contiguous for superpages. Implementations of this had TLB
entries for superpages where there were holes for missing base pages. Extra
level of address translations can also be added to allow OS to treat
non-contiguous physical pages as one superpage. However, because this requires
hardware support, its not implemented in general purpose processors and it
limits deployment.

## Design
This section details the reservation system proposed by the authors for
transparent and efficient superpage management. The goals of this design are to 
- Transparently support multiple superpage sizes 
- No need for application changes or specific hardware
- Control fragmentation without relocation
- Support promotion, demotion, and efficient eviction
- Scale to large superpages (4MB+)

The core components of this design are
- Buddy allocator: Physical memory is divided into blocks of power of two sizes
and is used to allocate contiguous memory regions efficiently
- Multi liste reservation scheme: Partially used reservations are tracked by
page size. This can simplify choosing the reservation to preempt if needed.
Each per size reservation list is sorted by LRU
- Population map: A radix tree that tracks which pages in memory are allocated.
It's used to look up reserved page frames, avoid overlapping reservations,
trigerring superpage promotion, and supporting reservation preemption

When there is a page fault, the OS tries to reserve a contiguous region aligned
to the desired superpage size. Only the used section is mapped and other pages
are reserved for future allocation. Superpages are chosen by preferring the
largest superpage fits the need. For fixed size objects like code and data
regions, the size must be within the bounds for but for dynamic objects like
the stack and heap, it can grow beyond the size. Reservations can also be
preempted when contiguous memory is low. Pages are also eviceted bin a
contiguity-aware manner to free up larger blocks. This controls the amount of
fragmentation that occurs. Pages that are reserved are promoted to super pages
incrementally when a subset of pages in the reservation are fully allocated and
aligned. As more allocation happens, pages are promoted to larger superpages.
Superpage demotion occurs when there is an eviction, protection change, or
memory pressure. This is to get around the fact that there is only one usage
bit per superpage so demotion allows OS to find which subpages are not being
used. To avoid having to flush entire superpages, clean superpages are demoted
before writes to allow finer grained tracking. The authors tried sha-1 hashes
but found it was too costly.

## Implementation notes
This section talks about the adjustments needed to implement superpage
management in FreeBSD. It was implemented as a loadable kernel module with
around 3500 lines of C code.

Contiguity aware page daemon: Daemon responsible for reclaiming memory when the
system is under pressure. It was modified to preserve and restore contiguity
instead of just freeing memory. The daemon is trigerred when there large
contiguous blocks are unavailable. It targets freeing pages that allow
contiguity. It also reclaims pages when the files that back the file backed
pages are closed. This allows early eviction

Wired pages clustering: Non pageable OS kernel structures can block large
superpages if they are scattered. Wired pages were clustered into dedicated
pools during allocation which prevents it from fragmenting physical memory.

Multiple mappings support: If two processes map the same file at different
virtual addresses, superpage alignment can be broken and only one mapping will
be aligned meaning only one of the processes can use a superpage. The solution
to this is to automatically select an aligned address that supports superpage
if the application doesn't specify the virtual address.

## Evaluation
The authors tested the superpage management strategy with CPU intensive
workloads, real world applications, and synthetic benchmarks. Superpages
provided substantial speedups when memory is plentiful and unfragmented. TLB
miss reduction was nearly 100%. Different applications perferred different
superpage sizes which showed that multi-size superpages was beneficial. Real
workloads (web server and background tasks) showed that the contiguity aware
daemon was able to recover contiguity and preserve superpage performance over
time. Worst case scenario testing showed that overhead ranged from 8.9% to 0.1%
which shows in worst cases, overhad is minimal.
