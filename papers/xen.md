# Xen and the Art of Virtualization

## Summary

## Introduction
Xen is a high performance VMM that supports resource management, performance
isolation, and multiple operating systems running simultaneously.
Virtualization is hard because VMs must be isolated, OSes can be very
different, and VMM overhead must be small. Xen multiplexes hardware at the OS
level and supports multiple operating systems running on it. Traditional OS
sharing has poor performance and is very complex to fit the OS to allow this.
Prior works such as exokernel influenced Xen's low level multiplexing. 

## Xen: Approach and Overview
The paper says that x86 was not designed for virtualization because certain
privileged instructions fail silently making them hard to trap and the MMU is
hard to virtualize efficiently. Other workaroudns include dynamic binary
rewriting but this has performance penalities. Xen uses paravirtualization, a
slightly modified hardware interface, avoiding th problematic parts of x86. Due
to this, guest OSes do have to be modified. The design principles of Xen is to
have
- Unmodified application support
- Support for ful OSes
- Use of paravirtualization
- Expose real resource characteristics

Xen is a hypervisor that runs beneath the guestOSes.

- Memory management: x86 has a hardware managed TLB making virtualization of
TLB hard. There is also no TLB tagging which means address flush every time.
Xen overcomes this by having guest OSes register new page tables with Xen on
creation. The guest OS relinquishes write access to the page table and all
subsequent updates are validated by Xen. Guest OSes can make batch updates. The
top 64MB of memory is reserved for Xen and is not accessible by the guest OS.
This avoids TLB flush when switching between guest and Xen. 
- CPU: The guest OS is no longer the most privieged. x86 has four privilege
levels. The guest OS runs at level 1 and applications run at level 3. Xen runs
at level 0. Privileged operations is emulated by Xen and attempts at direct
execution is trapped or failed. Exceptions are delivered by Xen to the guest OS
where the guest OS registers exception handler table with Xen. The exception is
with page fault where Xen captures the exception and passes on via exnteded
stack frame. 
- Device I/O: Emulating hardware is slow so Xen provies a virtaul device
interface that communicate with shared memory rings and async I/O. Xen has an
event notification system where events like hardware interrupts are delivered
as notifications via callbacks. Gues OSes also have the option to defer the
callback 

Price of porting existing OS: The effort needed to port an existing OS is
measurable with LOC. Linux required 2995 lines and XP required 4620. Some
systems require more effort to port such as Windows XP because there are
complex and numerous access points to page table entries and each of these had
to be rewritten. In Linux, preprocessor macros provided a centralized point to
change the translation logic. 

## Evaluation
The goals of the performacne evaluation is to compare performance with other
virtualization platforms, measure system throughput while running multiple
applications in separate VMs vs running all on the native OS, measure whether
Xen effectively isolates performance between guest OSes, measure the overhead
of using Xen. The guest OS was XenoLinux.

Relative Performance: This measures how close Xen got to native Linux.
Measurements show Xen has minimal performance degradataion while other
platforms like VMWare and UML have significant higher overhead. This because of
the paravirtualization method that redues trap overhead, efficient I/O handling
with shared memory and event based communication, and a clean abstraction layer
over virtual devices. Measurements show Xen has 1-3% overhead.

OS Benchmarks: These benchmarks were used to pinpoint the source of overhead in
Xen and compare it to native Linux and other virtualization methods. Xen's
microbenchmark performance is close to that of native Linux and overheads in
Xen are localized to sepcific operations with page table updates, page faults, 
context sitching, and network packet processing. The paper does analsis on
different subsystems. Process creation is slower in Xen due to verification of
page table udpates. 
