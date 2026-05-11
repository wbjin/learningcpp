# Limited Direct Execution

LDE is the illusion the OS provides to a user process that it is running
natively on hardware. For the most part, we want user processes to execute
instructions without the meddling of the OS. However, for certain instructions
we want the OS to be involved. This can be done with the combination of user
and kernel modes as well as traps.

## What happens before `main` is called

### In the kernel
1. Create Process Control Block (PCB). The kernel allocates a data structure
that tracks the state of the process such as its PID, register state, memory
mappings, and scheduling info
2. Create new address space:
The virtual address space of a process is simply its mapping from virtual
address to physical address and metadata describing regions of memory such as
code, heap, stack, static. New address spaces are created in two cases 1) when
a new process is created via `fork()` and 2) when a new program is loaded in
via `exec`. To create a new address space in Linux, first the `mm_struct` is
allocated. This is a memory descriptor that represents a process's virtual
address space and keeps tracks of information such as:
- Virtual Memory Area (VMA): Represents a contiguous range of virtual addresses
within a process's address space that has the same attributes such as
permissions and backing store such as disk. A process typically has many VMAs
such as its code segment, data segment, the heap, and the stack
- Page tables: The `pgd` field points to the top level page table for the
process
- Memory usage stats: Things like total pages that are mapped, usage counters,
and number of VMAs

When a process is newly created, it's page table entries are mostly empty
except for those mapped to the kernel's address space (Upper 1GB)

3. Set up VMAs:
If a process is created via `fork`, the kernel copies the parent's VMAs and
marks pages as read only to enable copy on write. If via `exec`, the kernel
builds a fresh memory layout with regions for the code, data (initialized
global data), BSS (uninitialized global data), heap, and stack. Each has it's
own VMA.
4. Load executbale
The standard binary format in Linux is Executable Loadable Format (ELF). It
contains headers so that the linker and loader can understand the binary. When
running an executable, the ELF loader will inspect the headers and for each
section header such as `.text` (code), `.data` (data), and `.symtab`  (symbol
table), it will create a VMA. However, pages are not loaded in memory yet and
they are marked as not resident No memory is allocated for the process yet.
When the process tries to access memory, the processor triggers a page fault
trapping into the kernel. The kernel allocates a physical page, loads the data
from disk, and updates the page table.
5. Set up stack and arguments
The kernel allocates the VMA for the stack and pushes the arguments `argc`,
`argv`, `envp` arguments into the process's stack.
6. Switch to user address space
The kernel sets the `cr3` register (x86-64) to the page table base register for
that process.
8. Execution:
The isntruction pointer is set to the program entry point (`_start`). The
privilege is lowered to user mode.

### C/C++ runtime
Once the kernel loads the process and executes it, the C runtime and the
dynamic linker takes over to eventually call `main`.
```
execve("./main", ["./main"], 0xffffdb943c20 /* 13 vars */) = 0
brk(NULL)                               = 0xc2d011bb0000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xf5acb7300000
faccessat(AT_FDCWD, "/etc/ld.so.preload", R_OK) = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=23083, ...}) = 0
mmap(NULL, 23083, PROT_READ, MAP_PRIVATE, 3, 0) = 0xf5acb72fa000
close(3)                                = 0
openat(AT_FDCWD, "/lib/aarch64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0\267\0\1\0\0\0\360\206\2\0\0\0\0\0"..., 832) = 832
fstat(3, {st_mode=S_IFREG|0755, st_size=1722920, ...}) = 0
mmap(NULL, 1892240, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_DENYWRITE, -1, 0) = 0xf5acb70f9000
mmap(0xf5acb7100000, 1826704, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0) = 0xf5acb7100000
munmap(0xf5acb70f9000, 28672)           = 0
munmap(0xf5acb72be000, 36752)           = 0
mprotect(0xf5acb729a000, 77824, PROT_NONE) = 0
mmap(0xf5acb72ad000, 20480, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x19d000) = 0xf5acb72ad000
mmap(0xf5acb72b2000, 49040, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xf5acb72b2000
close(3)                                = 0
set_tid_address(0xf5acb7300f90)         = 1996
set_robust_list(0xf5acb7300fa0, 24)     = 0
rseq(0xf5acb73015e0, 0x20, 0, 0xd428bc00) = 0
mprotect(0xf5acb72ad000, 12288, PROT_READ) = 0
mprotect(0xc2cff414f000, 4096, PROT_READ) = 0
mprotect(0xf5acb7305000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0xf5acb72fa000, 23083)           = 0
exit_group(0)                           = ?
```
1. `execve`: The kernel replaces the current process image (that of the shell
likely) with one for ./main. This creates a new address space and loads the ELF
2. `brk`: A syscall used to change the amount of space allocated for a
process's heap by setting the location of the program break (the address
that defines the end of the heap, right before the BSS segment). Calling `brk`
with `NULL` gives back the current program break address.
3. `facesssat("/etc/ld.so.preload)`: Checks whether any libraries should be
forcibly preloaded.
4. `openat("...libc...)`, `read`, `fstat`: Opens the libc ELF file, reads the
ELF headers, and gets the file metadata.
5. `mmap`s: Reserves a contiguous virtual address range for libc and maps
libc's text segment into the process's memory
6. `mprotect`: Creates a protection region around libc
7. `mmap`s: Maps libc's writable data segment
8. `set_tid_address`, `set_robust_list`, `rseq`: Sets up threading. 
9. `mprotect`s: Marks relocation and metadata regions as readonly after dynamic
linking
