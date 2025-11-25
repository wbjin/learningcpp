# Firecracker: Lightweight Virtualization for Serverless Applications

## Summary
Firecracker is a VMM specifically for serverless computing and container
workloads. Firecracker is a lightweight VMM that is optimized for running
thousands of MicroVMs on a single host, minimal memory overhead, and boot times
under 125 ms. 

## Introduction
With the rise of serverless computing and containers, multitenancy is more
important. Hypervisors offer full isolation but has high overhead. Containers
are fast and lightweight but provide weaker isolation guarantees. Existing
approaches compromised between compatibility, startup time, security, and
overhead. Firecracker is a new VMM using KVM but replacing QEMU. It is
optimized for serverless workloads with fast startup, minimal resource
overhead, and high density with thousands of workloads per machine.

## Choosing an Isolation Strategy
Before Firecracker, AWS Lambda used containers within a customer and VMs
between customers. However, this had poor workload packing due to fixed VM
sizes. The authors evaluated various methods for isolation
- Linux Containers: Using cgroups, namespaces, and seccomp-bpf for isolation.
This was lightweight and had fast start times but since everyone shared the
same host kernel, there was a large surface area. Syscalls had to be limited
which meant some workloads were not compatibile and it was vulnerable to
side-channel attacks.
- Language Virtual Machines: Language virtual machines such as JVM and V8
allowed apps written in the managed languages. However, it doesn't work for
arbitrary Linux binaries.
- Virtualization: Using hardware based isolation such as Intel VT-x had strong
isolation and could run unmodified OSes allowing all kernel features. However,
it had high memory and CPU overhead and a slow startup time.

Firecracker chose virtualization but not like traditional VMMs. Firecracker
kept KVM, replaced QEMU with a minimal Rust based VMM.

## Firecracker VMM
Firecracker leverages KVM to launch and manage microVMs. Firecracker is very
minimal and doesn't contain a BIOS, PCI, or complex device emulation. It
delegates scheduling, memory management, and I/O to the Linux kernel isntead of
duplicating OS features. Each MicroVM runs its own Firecracker process that
treats all guest code as untrusted. It only supports virtual I/O devices (block
storage, network interfaces, serial ports) and Firecracker VMs are managed
through a REST API that controls the guest kernel and disk image, network
config, rate limiters, and VM start/stops. Network and disk devices support
rate limiting and is enforced within Firecracker. To mitigate side channel
attacks. HyperThreading is disable, kernel page table isolation is enforced,
shared file usage is prevented, and a minimal syscall surface is maintained.

## Firecracker in Production
Firecracker powers every AWS Lambda invocation by running each function inside
a dedicated MicroVM and providing isolation, fast startup, and low overhead.
Each Lambda worker host runs hundreds to thousands of MicroVMs. Customer
functions are defined as slots and can be rused across many invokations of the
same function. Each MicroVM runs a minimal Linux Kernel, a shim process for
communication and the host runs a micro manager that manages the MicroVMs and
does monitoring and logging. MicroVMs are kept in VM pools to avoid cold start
ups

## Evaluation
Firecracker boot times were two times faster than QEMU and with 50 VMs, scaled
better than cloud hypervisor. VM Overhead was much smaller than both Cloud
Hypervisor and Qemu. Firecracker's disk I/O performance wasn't as fast as Cloud
HV and QEMU but it was adequate for Lambda's needs. Network throughput was
worse than Cloud HV and QEMU. 
