# Segmentation In Hardware

x86 has three kinds of addresses
- Logical: x86 divides the memory model into segments. Contains the segment and
an offset within that segment. Programs use this address to refer to data.
- Linear: Essentially virtual addresses. Logical addresses are translated to
linear addresses.
- Physical: Address of memory cells in memory chips. Linear addresses are
translated to physical addresses.

The MMU contains:
- Segmentation unit: Logical address -> Linear address
- Paging unit: Linear address -> Physical address

Read and write need to be performed serially on RAM chips. RAM chips thus have
a memory arbiter that serializes reads and writes even on a multiprocessor
system.

The logical address consists of a segment identifier (called the segment
selector) and an offset. The segment selector is 16 bits and the offset is 32
bits. These segmenet selectors are placed in segment registers whose only
purpose is to hold segment selectors. These are `cs, ss, ds, es, fs, gs`. `cs`
is specifically for pointing to a segment containing the program instructions,
`ss` is for the program stack, and `ds` is the global and static data. The `cs`
register also contains a 2 bit field that specifies the processor's Current
Privilege Level (CPL). There are 4 different privilege levels but Linux only
uses 0 for kernel mode and 3 for user mode.

## Segment Descriptors
Each segment has an associated 8 byte Segment Descriptor that describes its
characteristics. These segment descriptors are located in either the GLobal
Descriptor Table (GDT) or Local Descriptor Table (LDT) where there is one GDT
system wide and each process can have its own LDT to create additional
segments. The segment descriptor contains information such as 
- Linear address (VA) of first byte of segment
- Offset of last memory cell (length basically)
- Flag to indicate system data  like GDT or LDT
- Segment type (Code, data, etc)
- Minimum privilege level needed to access segment

The different segment descriptor types that are used are Code Segment
Descriptor, Data Segment Descriptor (stack segments are implemented using
this), and Task State Segment Descriptor which refers to Task State Segment
that contains contents of processor registers, Local Descriptor Table
Descriptor that refers to a segment containing the LDT.

### Nowadays
In x86 32 bit, Linux still uses segments. In x86 64 bit, most segmentation is
disabled and only a few segments are still meaning full which are FS and GS
that conatain thread local storage. The TSS is still used to hold the stack
pointer. Segmentation is largely disabled, only the GDT is used with a few
entries and LDT is basically obsolete. Paging is the primary memory protection
mechanism.

## Fast Access to Segment Descriptors
The segment selector contains an index, table indicator, and requestor
privilege level. The index is used to calculate the offset into the GDT/LDT,
the table indicator indicates GDT or LDT, and the requestor privilege level is
the CPU privilege at the time of request. A segment selector is loaded into the
segmentation register and the corresponding segment descriptor is loaded into
the `cs, ss, ds` non-programmable registers. Subsequent segmetnation can use
the register instead of the GDT.
