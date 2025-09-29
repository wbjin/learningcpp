# My VM is Lighter and Safer than your Container

## Summary
LightVM demonstrates that VMs can be lightweight as containers while having
stronger isolation. Containers are thought to be fast and resource efficient
but doesn't have strong isolation. VM has strong isolation but is slower and
heavier. LightVM is a rearchitectued system based on Xen that boots in 2.3 ms
and has high VM desnity (8000 per host). 

## Introduction
Containers hav fast installation and have small memory footprints but has weak
isolation. It relies on the kernel syscall interface which there many of and is
difficult to secure. VMs provide strong isolation but take longer to boot and
require large amounts of memory and storage. The paper proposes a VM that is
lightweight as containers but actually provide strong isolation. The paper does
this by
- Analyzing existing bottlenecks in VMs
- Eliminating XenStore for VM management
- Decoupling VM prep from execution
- Creating a tool for generating Linux VMs

## Requirements
The following were the goals for LightVM
- Fast instnatiation, ms boot times
- High instance density, thousands of VMs per machine
- Pause and unpause support, allow this for VMs

The main bottleneck with these features is VM size which includes the image and
the memory needed in the image. This can be hunders of MBs to GBs. This large
size makes it slow to read and initialize and creates large memory footprints
limiting the number of VMs per host. To create lighter VMs, smaller images,
faster boot processes, and efficient control planes are need.

## Lightweight VMs
The two main approaches to lightweight VMs are:

Unikernels: Combine a minimal OS with the application itself combined into a
single VM image. This runs a single application only such as a TCP server. This
has execllent performance but writing new unikernels requires porting of
applications.

Tinyx: A custom build system fro creating a minimul Linux based VM tailored for
a specific application. This targets apps that can't be easily rewritten for
unikernels. Tinyx taks the application and the platform (such as Xen) as the
input and outputs a Linux distribution with a custom kernel that automatically
includes the application and its runtime dependencies. This excludes things not
needed such as kernel modules or drivers for unneeded functionalities and it
does this by having a blacklist and whitelist of packages to trim from. The
kernel thus becomes much smaller and RAM requirements are much lower.

Both approaches covers a wide range of use cases. Unikernels for applications
that need the extra optimization and Tinyx for applications that don't need
necessarily need the performance but want the ease of porting.

## Virtualization Today
This section examines current virtualziation systems like Xen and how they
perform with lightweight VMs. The paper especially focuses on Xen. Xen is a
hypervisor that manages CPU and memory. It has a driver domain where a special
VM with Linux has its toolstack, XenStore, back-end drivers, and software
switches. There is a guest VM domain that has the front end drivers and
communicates with the driver domain with shared memory and event channels. An
experiment the paper ran was 3 VM types (Debian, Tinyx, Unikernel) to measure
the creation time and the boot time.
- Debian VM: Creation 500ms, boot 1.5s
- Tinyx VM: Creation 360ms, boot 180ms
- Unikernel: Creation 80ms, boot 3ms
- Docker: Boot 200ms
As VM instances grew, lightweight VMs hit a bottleneck but Docker containers
and normal processses didn't degrade much with scale

The key bottlenecks the authors identified were
- XenStore: A shared inofrmation storage space for configuration and status of
all guest VMs and a centralized management point for the driver domain.
XenStore had high software interrupts and switching overhead, every operation
required a domain crossing, expensive transactions and comparisons, and log
file rotations that caused performance spikes. The conclusion was that XenStore
was too slow and complex for lightweight VMs
- Device Creation: Creating virtual devices took time

As VMs became smaller, Xen's control plane became the bottleneck. For fast
boot, Xen's toolstack and XenStore needed to be redesigned or eliminated.

## Light VM
LightVM rearchitected Xen by replacing XenStore, the toolstack, and
introducting split toolstacks for creation and booting and also optimizing
device setup.

NoXS: No XenStore eliminates XenStore and replaces inter-VM communication with
shared memory pages and direct event channels instead of going through
XenStore. Information needed by guest VMs like backendIDs, grant refs are
stored in a device memory page managed by the hypervisor. THe tool change was
replacing `xl/libxl` with `chaos/libchaos` for a lightweight command line tool
and library that directly interacts with the hypervisor using `ioctl`. This
eliminated the dependency on XenStore

Split Toolstack: Many parts of VM creation such as reserving memory and
assigning CPUs is common across VMs so this can be precomputed and cached.
There is a new prepare phase that happens offline preiodically where VM shells
were created and when VMs needed to be created on demand, the execute phase tol
the VM common config and finalized it with app specific config.

xendevd: Xen uses bash scripts ot configure devices but since scripts are slow,
this was replaced with xendevd which was a binary daemon that responded to
backend events and handled device setup in C

## Evaluation
To evaluate LightVM, it was compared against standard Xen, Docker containers,
and Linux processes across boot and creation times, scalability, migration and
checkpointing, and memory and CPU usage.

Boot 1000 Unikernel VMs:
- Standard Xen: Went from 100ms in the first one to 1s
- Chaos + XenStore: Went from 15ms to 80ms
- Chaos + split toolstack: Max 25ms
- Chaos + noxs: 8-15ms
- LightVM: 4-4.1ms

In comparison with docker, LightVM could run 8000 VMs while docker hit a memory
bottleneck at 3000 containers and boot time degraded very quickly.

Checkpoint and migration: LightVM had much fater migration time around 60ms 

Memory footprint: For 1000 instances
- Unikernel: 5 GB
- Tinyx: 27 GB
- Debian: 114 GB
- Docker: 5GB
- Linux processes: 5Gb

CPU Utilization: 1000 instances
- Unikernel: 0.3%
- Tinux: 1%
- Debian: 25%
- Docker: 0.2%

## Use Cases
In this section, the authors demonstrate real world applicability of LightVm 

Personal Firewalls for Mobile: Mobile devices are under increasing threat and
firewalls on phones are insufficient. LightVM can be used to place firewalls in
base stations where isolation between users are important. This would allow
scaling of these firewalls but also quick boot times.

Just in time service installation: Services should start on demand and
terminate when not used. This means boot VM only when client connects and shut
it down after inactivity. 

High Density TLS Termination: CDNs terminate TLS connections close to users and
need fast boot but high isolation.

Lightweight Compute: Services like AWS Lambda run short lived compute functions
with strong isolations. LightVM can be used to quickly set up and tear down
these server functions while providing high isolation
