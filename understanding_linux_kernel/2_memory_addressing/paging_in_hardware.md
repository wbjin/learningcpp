# Paging in Hardware

Terminaology:
- Page frame: A physical page on RAM
- Page: A block of data that can be in RAM or disk. Size of a page is same as
the size of a page frame

x86 processors support paging with the `PG` flag in the `cr0` control register.
When `PG = 0`, linear addresses are interpreted as physical units.

## Regular Paging
The pagining unit of Intel processors handle 4KB pages where a 32 bit linear address is brokne into
- Directory: First 10 bits
- Table: Middle 10 bits
- Offset: Last 12 bits

Translation occurs in two steps. First translating through the Page Directory
and then the Page Table. This is essentially a two level page table to reduce
the number of page table entries neeeded to represent a 4GB address space. Each
process has a Page Directory assigned to it and pages are allocated in RAM on
demand. The physical address of the Page Directory is stored in the `cr3`
register. The entry in the Page Directory poitns to the Proper Page Table and
the addresse's table filed is used to index into the Page Table. There can be
1024 Page Directory entries, 1024 Page Table entries, and 4096 bytes per page
which means each Page Directory can address up to 1024 * 1024 * 4096.

The Page Directory and the Page Table have the same entry structure.
- Present flag: When set, indicates it is contained in main memory
- Physical address what it is pointing to: The 20 most significant bits of a
page frame address. Because each page frame is 4KB, physical addresses need to
be multiples of 4096 which means the last 12 bits of an address is always 0
(not needed). If it is a Page Directory, it points to a page table. If it is a
page table, it points to a page frame containing page data
- Accessed flag: Set each time paging unit addresses corresponding page frame.
OS must clear this, it is only set by the paging unit
- Dirty flag: Set each time a write operation is performed on page frame. Never
clears this flag, OS must clear it
- Read/Write flag: Read and write or just read
- User/Supervisor flag: Privilege level required to access the page or Page Table
- PCD and PWT flags: controls the way page or Page Table is handled by hardware cache
- Page size flag: Only for Page Directory entries. If set, entry refers to 2 MB
or 4MB long page frame
- Global flag: Applies only to Page Table entries. Used to prevent frequently
used pages from being flushed from TLB cache. Only works if Page Global Enable
`PGE` flag of `cr4` register is set

## Extended Paging
From Pentium model, Intel processors started support for 4MB page frames as
well as the standard 4KB. For these 4MB page frames, there is no intermediate
Page Table and the Page Directory entry points directory at page data. This can
be used by setting Page size flag in Page Directory entry.
- Directory: First 10 bits
- Offset: Remaining 22 bits

Only the 10 most significant bits of the 20 bit physical address field is
significant because physical addresses aligned on 4MB boundaries so the least
22 significant bits are always 0.

Extended paging can be enabled by setting the `PSE` flag in the `cr4` register.

## Hardware Protection Scheme
Page level control access can only be done in two groups. When the
User/Supervisor flag is 0, the page can be addressed only when the current
privilege level is less than 3 (when process is running in kernel mode).
Otherwise, the page can always be accessed. If the Read/Write flag is cleared,
the page table or page can only be read.

## An example
Consider a process with the linear address space between 0x20000000 and
0x2003ffff. This space has exactly 64 pages. The first 10 bits is 0x080 or 128
in decimal meaning this process uses the 129th directory entry in the Page
Directory. The entry in the Page Directory contains the physical address of the
Page Table assigned to the process. If no other linear addresses are assigned
to the process, the remaining 1024 entries are zeroed out.

The table filed values ranging from 0 to 0x03f or 0 to 63 in decimal indicate
that only the first 64 entries of the Page Table is valid. For the address
0x020021406, the 0x21 page table offset is used to find the address of the page
frame. Then the 0x0406 offset is addeded to the page frame physical address. If
the present flag is cleared, the paging unit issues a page fault exception.

## Physical Address Extension 
The amount of RAM a processor can have is limited by the number of address pins
connected to the address bus. Requirements for larger address spaces for the 32
bit x86 architecture made intel increase the number of address pins from 32 to
36 allowing 64 GB RAM. To handle paging with a 36 bit address space, Intel
introduced Physical Address Extension. It is activated by setting the `PAE`
flag in the `cr4` register.

The 64GB RAM is split into 2^24 page frames meaning the physical address field
of Page Table entries was expanded from 20 to 24 bits. This means that each
Page Table entry needs to support 36 bits for addressing so each Page Table
entry was increased to 64 bits for a total of 512 entries.

A new level of Page Table called Page Directory Pointer Table consisting of
four 64 bit entries was introduced. The `cr3` register contains a 27 bit PDPT
base address field. When translating virtual addresses, the 32 bit address is
interpreted as 
- 31-30: Point to one of four PDPT
- 29-21: Point to 1 of 512 Page Directory entries
- 29-12: Point to 1 of 512 Page Table entries
- 11-0: Offset into 4KB page

With 2MB page sizes (`PS` flag set), the page directory is excluded
- 31-30: PDPT entry
- 29-21: Page Directory entry
- 20-0: Offset into 2 MB

## Paging for 64-bit Arch
Two level page table is not sufficient for a 64 bit address space. Thus, more
levels of page tables are needed. 64 bit x86 uses 4 page tables with 4KB page
sizes with 48 of the 64 bits being used. 9 bits are used for each level of page
table and 12 for the offset.

## Hardware Cache
DRAM is used for main memory, the fast SRAM (static RAM) is used for caches.
Caches are subdivided into lines of a few contiguous bytes that can be
transferred in burst between memory and cache. Direct mapped caches is where a
line in main memory is stored at the exact same loation in the cache. Fully
associative caches is where a line in memory can be stored at any location the
cache. Most caches are N-way set associative where any line of main memory can
be stored in any one of N lines of the cache.

The cache unit is inserted between the paging unit and the main memory. It
contains a hardware cache and a cache controller where the memory stores the
actual data and the controller stores an array of entries, one for each line on
the cache memory. Each line has a tag and a few flags that describe the status
of the cache line. This tag consists of bits that allow the cache controller to
recognize the memory location that is mapped to this line. There are usually
three groups, the most significant group that corresponds to the tag, the
middle one for cache controller subset index, and the least significant for the
offset within the line. To check for cache entries, the CPU extracts the subset
index from the physical address and compares the tags of lines in the subset
with the high order bits of the physical address.

On a cache hit for a read, the cache controller will transfer it into a CPU
register. For a write, the cache controller can have two strategies,
write-through or write-back. Write-through is where the cache-controller always
writes to both DRAM and cache. In write-back, the cache controller will waiting
until the cache line is evicted.

Multiprocessors have separate caches for every processor and need hardware
support for cache consistency. There are also numerous levels of caches.
However, cache consistency is implemented in hardware and Linux assumes a
singular cache.

The `CD` flag in the `cr0` register enables or disables cache circuitry and the
`NW` flag specifies whether to write through or write back.

## Translation Lookaside buffer
The TLB is a specialized cache for speeding up address translation. When a
virtual address is translated for the first time, the corresponding physical
address is stored in a TLB entry so future references can be quick. In
multiprocessors, each CPU has its own TLB. These TLBs do not need to be
synchronized because processes running on different CPUs may associated the
same virtual addrses to a different physical address. When the `cr3` register
is modified, the hardware automatically invalidates all entries of the local
TLB.
