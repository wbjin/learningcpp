# Dune: Safe User-level Access to Privileged CPU Features

## Summary
Dune is a system that enables user level appliations to safely access
privileged CPU features. A process runs in "Dune" mode where it runs in VMX
non-root where syscalls are made via VMCALL. This allows of sandboxing of
processes while minimizing performance overhead. Dune is not a full VM but it
bridges the gap bewteen full isolation at the cost of overhead and no isolation.

## Introduction
Some applications need the direct access to hardware features such as page
tables and ring protection which are traditionally restricted only to the
kernel. Currently, the kernel needs to be modified to do so or users can run
full VMs which offers hardware access and isolation but that is too overkill
for requiring just a handful of privileges. Dune exposes a process abstraction
using hardware virutalization. It leverages Intel VT-x to give direct user
level access to CPU features while maintaining compatibility with standard
Linux process interfaces. Dune has a kernel module and a user level library
called libDune through which privileged hardware can be used. A process using
Dune goes into Dune mode where syscalls are made with VMCALL.

## Hardware and Virtualization
Intel VT-x is a hardware extension that simplifies and accelerates
virtualization. Instead of trapping for every privileged instruction, it lets
most instructions run directly on hardware and maintains a shadow copy of
privileged state for safe execution. VT-x has different operating modes. Root
mode for the host/VMM, non root mode for the gues which is Dune's user level
process, and VM entry/exit where transitions are handled by hardware such as
saving and restoring VMCS. The VMCS defines which instructions cause a VM exit
and which hardware is exposed to the guest. The Extended Page Table allows safe
virtualization of memory where there is a second layer of address translation.
Even if the guest changes its own page tables, there is still a layer managed
by the VMM/host kernel that the guest can't change. Dune exposes hardware
features such as exceptions, virtual memory, and privileged modes to users
while maintaing isolation. This is because even with direct access, EPT
enforces memory isolation, system calls require a VM exit and the kernel to
handle it, and untrusted code can be sandboxed within Dune.

## Kernel Support for Dune
Dune is a loadable kernel module in Linux that manages Intel VT-x and VMCS
structures per process. It safely exposes privileged hardware through the
VMCALL interface while preserving compatibility with the Linux system calls. A
process enters dune mode with an `ioctl` on `/dev/dune` and once it is in Dune
mode, it is running in VMX non-root mode. Once a process is in Dune mode it
cannot exit. VM exists occur on system calls or faults and the kernel switches
over to take care of this. `libDune` is a user level library that helps with
interfacing with Dune and handling privileged features. Dune doesn't weaken the
existing OS security model. A Dune process can do anything it oculd do while it
was a normal Linux process but now there is an adidtional privilege separation
within user space. To allow users to manage their own page tabels while
ensuring security and isolation, Dune uses the EPT feature that allows a two
level translation hierarchy where users can only see the guest virtual to guest
physical translation and it cannot see the guest physical to host physical
translation. To expose hardware, each process has a VMCS that mirrors things
like registers and data structures from the kernel.

## User-mode Environment
Dune processes run in the VMX non root mode which is x86 ring 0. Despite this,
it is safe because hardware enforcement and EPT-based isolation. Syscalls are
made through VMCALL and illegal syscall usaes are detected and translated to
hypercalls. `libDune` contains things like page table manager, ELF loader, page
allocator, syscall and exception handlers all in the user space. It is a
utility library for users to interface with Dune. Dune also has a modified libc
that replaces syscalls with VMCALL. Starting a Dune process is like starting an
OS. Application builds a custom page table and issues an ioctl to enter Dune.
Dune switches the process to VMX non-root mode. Some limitatinos are that there
is incomplete signal support because libDune currently requires a special
signal API. Thread safety is an issue where parts of libDune like the page
table manager is not thread safe. Some syscalls also expect host virtual
address but Dune provides a guest virtual address which can cause issues.
Because Dune exposes architecture specific hardware features it reduces
protability with other architectures.

## Application
Some real world applications of Dune

Sandboxing: Run untrusted binaries securely by running them in a sandbox
environment in ring 3. 

Wedge: A system that isolates components using sthreads (lightweight threads
with custom isolation policies)

Garbage Collection: Use Dune to enhance a garbage collector that does mark and
sweeping. 

## Evaluation
The overhead introduced by dune is from VMX transitions in syscalls and faults
as well as EPT overhead because there is an additional alyer of address
translation. However, Dune greatly reduces some software overhead by allowing
applications to directly use hardware. For I/O heavy workloads like a
webserver, Dune perserves native I/O performance while VMs have much lower
performance.
