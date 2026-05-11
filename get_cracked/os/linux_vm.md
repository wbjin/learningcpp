# Linux Virtual Memory

The virtual address space of a linux process is split into a user portion and a
kernel portion. In 32 bit linux, the lower 3GB (0-0xBFFFFFFF) is the user
portion and the upper 1GB (0xC0000000-0xFFFFFFFF0) is the kernel portion. In 62
bit linux, the portions are split equally with each portion taking up 128TB.
The user portion goes from 0x0000000000000000-0x00007FFFFFFFFFFF and the kernel
portion goes from 0xFFFF800000000000 to 0xFFFFFFFFFFFFFFFF. The hole in between
these two portions is considered invalid.

In x86 Linux, the kernel portion is split into two types of virtual addresses.
The first is the kernel logical address which is what you would consider the
kernel's "regular" virtual address space. Calling `kmalloc` allocates memory in
the logical address space. Most kernel data structures such as page tables and
per process kernel stacks live in this area. The logical address also has a
fixed direct mapping to the lower part of physical memory. In 32 bit Linux,
0xC0000000 maps to 0 physical address and 0xC0000FFF maps to 0x00000FFF and so
on. In 64 bit Linux, the start of the logical addresses (0xffff888000000000)
maps to physical address 0. Direct mapping makes address translation simpler
and reduces TLB pressure. It also guarantees physical contiguity which improves
performance DMA from I/O devices more efficient.

The other type of kernel virtual address space is the kernel virtual address.
This is the virtually contiguous region of memory that is non-contiguous in
physical memory. This allows the kernel to allocate big data structures in
cases where contiguous physical memory is hard to find. `vmalloc` is the
allocator for the virtual address part.

With 64 bit x86, the 48 bits out of the full 64 bit address is used for
accessing memory. The bottom 12 bits are used for an offset into 4KB pages and
the remaining 36 bits are used for indexing into a multi-level page table.
Linux on x86 64 bit defaults to a 4 level page table with 9 bits used for the
offset at each level. Intel x86 supports page sizes of 4KB, 2MB, and 1GB so
huge pages are supported. When huge pages are used, the page table is condensed
to a three level page table.

Linux uses a page cache that backs file backed memmory (`mmap`) and files
(`read/write`). The kernel stores the contains of files and block devices in
physical memory when doing reads or writes. When you issue a `read`, it is
actually operating on the page cache instead of the actual file. The kernel
loads the data into memory if it is missing transparently.
