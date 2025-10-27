# Segmentation In Linux

Linux doesn't really use segmentation. Segmentation and paging is redundant and
both Linux prefers paging because it makes memory management simpler and makes
it more portable especially to RISC architectures that have limited support for
segmentation. Linux uses segments to identify the user code and user data
segments when the processor is running in user mode and the kernel code and
kernel data segments when the processor is running in kernel mode. In Linux,
the base and limit of all of these segments are the same. This means that
logical addresses and linear addresses coincide where the offset of the logical
address is the linear address. Because there is only one segment for data and
one segment for code in both privilege modes, the kernel doesn't have to use
the segmentation register to choose the segment. When loading or storing data,
it is implicitly referring to the user/kernel data segment when executing code,
it is implicitly user/kernel code segment and the registers that contain these
segment descriptors are known (`cs` for code and `ds` for data)

## Linux GDT
In uniprocessor systems, there is one GDT while in multiprocessory systems,
there is one GDT per processor. All GDTs follow the same layout with 18 segment
descriptors and 14 null, unused or reserved entries. The used segment
descriptors in each GDT are
- Kernel/User code and data: 4
- Task State Segment: Refers to TSS used to store register data, 1
- LDT: Refers to LDT, 1
- Thread Local Storage: Store data local to each thread, 3 
- Advanced Power Management: 3
- Plug and Play BIOS: 5
- Speciall TSS: For double faults, 1

## Linux LDT
LDTs are shared by most processes since Linux user programs don't use an LDT.
Some applications however do require custom segments.

### Nowadays
- The exact layout of the GDT may be different.
- LDT is pretty much unused and ignored and support for it is to maintain
backwards compatibility
- In x86 64 bit, segmentation is pretty much disabled, the only use for it is
thread local storage
