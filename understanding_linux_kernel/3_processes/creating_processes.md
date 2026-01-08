# Creating Processes

Unix kernels makes process creation more efficient by doing
- Copy on Write: Parent and child process shares the same physical page frame
until one tries to write at which point a new page is allocated for the process
that is initiating the write. Old data is copied over.
- Lightweight processes: Using threads instead of full processes for
concurrency because LWP share the same kernel data structures such as paging
tables and open file tables
- `vfork()` system call creates processes that sahre the same memory address
space as its parent

## The clone(), fork(), and vfork() system calls
Lightweight processes are created with the `clone()` system call. `clone()` is
a wrapper function defined in the C library. It uses the `sys_clone` system call
which doesn't actually have the function and argument as parameters. Instead,
the wrapper saves the pointer to the function in the child's stack position
corresponding to the return address of the wrapper function and the arguments
right after such that the function is executed after the wrapper function
terminates. The caller of clone needs to manually allocate memory for the
child's stack. Depending on the arguments passed into the syscall, the stack
may remain in the parent's address space or not. The `CLONE_VM` flag allows the
processes to share the same memory descriptor struct and thus the same address
space. If this flag is not set, a new address space is created for the child.
In fact, the `fork` syscall is implemented using `clone` but with all the clone
flags cleared and the stack pointer pointing to the parents current stack
pointer. The `vfork` syscall is also implemented with `clone` but with the
`CLONE_VM` and `CLONE_VFORK` flags set.

### do_fork function
The `do_fork()` function handles the `clone`, `fork`, and `vfork` system calls.
It does the following
1. Allocates a new PID for the child by looking at `pidmap_array`
2. Checks `ptrace` field of parent to see if the parent is being traced. If it
is, check whether debugger wants to trace the child and set flags
accordingly.
3. Call `copy_process()` function to make a copy of the process descriptor.
Returns the address of the `task_struct` descriptor created
4. If `CLONE_STOPPED` flag is set or child process is being traced, sets child
to `TASK_STOPPED` and waits until the parent sets it to `TASK_RUNNING` with
the `SIGCONT` signal
5. If `CLONE_STOPPED` not set, invokes `wake_up_new_task()`. This function
adjusts the scheduling parameters of parent and child. Additionally, if
child will run on the same CPU as the parent and child does not share same page
table (`CLONE_VM` is cleared and it is a fork), forces child to run before the
parent which allows better performance in cases where child executes `execve`.
If different CPU or `CLONE_VM` is set, child is put in the last position of the
runqueue the parent is in
6. If `CLONE_STOPPED` is set, puts child in `TASK_STOPPED`
7. If parent process is being traced, stops current process and sends a SIGCHLD
to its parent. The grandparent (the debugger) will see that the traced
parent created a child
8. If `CLONE_VFORK` is set, puts the parent in the wait queue and suspends it
until the child terminates or executes a new program
9. Returns the PID of the child

After `do_fork()`, there is a complete child process in the runnable state but
isn't actually running. This is done in a future process switch. The new child
process then starts executing right at the end of the syscall that was used to
create it. The value to return by the syscall is contained in the `rax`
register where it is 0 for the child and the PID of the child for the parent.

### `copy_process()` function
`copy_process()` is responsible for setting up the process descriptor needed by
the child process. 

## Kernel Threads
Kernel threads are used to perform critical tasks such as flusing disk cache,
swapping unused pages, servicing network connections. This is done in the
background as a separate process. Kernel threads differ from regular processes
in that they only run in kernel mode.

### Creating a kernel thread
`kernel_thread()` function creates a new kernel thread, taking in a kernel
function, arguments, and clone flags. It essentially invokes `do_fork` like
this
```C
do_fork(flags|CLONE_VM|CLONE_UNTRACED, 0, pregs, 0, NULL, NULL);
```

`CLONE_VM` is used to avoid duplicating page tables of the parent process. This
duplication is not needed because the new kernel thread will not access the
user mode address spac, only the kernel mode address space. Once the passed in
kernel function terminates, the kernel thread will exit invoking the `_exit()`
system call.

### Process 0
Process 0, also called the idle process, swapper process, is the ancestor of
all processes. It is a kernel thread created during the initialiation phase of
Linux. It uses the following statically allocated data structures while all
other processes use dynamically allocated data structures
- Process descriptor 
- Thread info descriptor in kernel mode stack
- `init_mm`, `init_fs`, `init_files`, `init_signals`, `init_sighand`
- Master kernel page global directory

Process 0 then initializes all data structures needed by the kernel, enables
interrupts, and creates another kernel thread called process 1 or the init
process. Then, it execues the `cpu_idle` function which continously executes
the `hlt` (halt) instruction which stops the CPU that the process is running
on until an interrupt. Process 0 is only scheduled again when no other process
are in the `TASK_RUNNING` state. Multicore systems have a process 0 for each
CPU. After power is turned on, the BIOS starts a single CPU and creates process
0 to initialize the kernel data structures. Then, the other CPUs are enabled
and copies the process using `copy_process`

### Process 1
Process 1 also known as the init process executes the `init()` function which
completes the initialization of the kernel. This function calls `execve` into
`init` program making it its own process with its own per-process kernel data
structures. The init process stays alive until the system is shut down.

### Other kernel threads
- keventd: Executes the functions in the `keventd_wq` workqueue
- kapmd: Handles events related to Advanged Power Management
- kswapd: Reclaims memory
- pdflush: Flushes dirty buffers to disk to reclaim memory
- kblockd: Executes functions in `kblockd_workqueue`. Periodically activates
block device drivers
- ksoftirqd: Runs tasklets, one for each CPU
