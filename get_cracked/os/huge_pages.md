# Huge Pages

Modern server CPUs have TLB sizes of 1500-2000 entries. With 4KB pages, this is
roughly 8MB worth of memory. Using huge pages (2MB and 1GB sizes) increases the
addressable memory contained in the TLB to 4GB. Bigger pages also reduces the
number of page table entries and thus the size of page tables. This reduces
memory footprint but also makes it easier to cache page tables. Furthermore,
Linux uses three level page tables with bigger pages (2MB) which further
reduces the number of loads needed to translate addresses in TLB misses.

To allocate a 2MB page, Linux has to first find 512 4KB pages that are
contiguous. When there is high external fragmentation in the system, Linux has
to move around the physical pages of processes to create a contiguous block of
memory. This requires stopping the processes that are affected making it quite
disruptive.

Transparent huge pages is Linux's way of making huge pages more accessible
without the need for a change in the application program. When THP is in use,
the kernel will transparently take contiguous 4KB virtual pages (that are
backed by 512 contiguous physical page) and map them to a single 2MB page
instead of 512 separate 4KB entries. This reduces pressure on the TLB, reduces
the size of page tables, and reduces the load on TLB misses. When there is high
memory load on the system, the kernel will automatically break apart the huge
page to create more pages. THP has two modes, the always and madvise modes. In
always mode, most memory allocated by the program is eligible to be backed by
huge pages. In madvise mode, the program needs to explicitly tell the kernel
with the madvise system calls to use huge pages for certain parts of memory.

`hugelbfs` is also another way to use huge pages. This uses a pool of huge
pages (where a minimum number of huge pages are pre allocated), making
allocation by the program as fast as allocating a 4KB page.
