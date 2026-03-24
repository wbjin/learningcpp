# Full Stack

NetFilter is hooks that can be attached at layers of the network stack for
filtering, especially used for firewalls.

## User Space vs Kernel Space
NIC drivers run in the kernel space and operate on hardware registers and
memory regions to control the NIC. The NIC exposes control registers via
memory-mapped I/O or PCIe. Memory-mapped I/O makes a device's hardware
registers appear as normal memory addresses to the CPU. Loads and stores to
these addresses are essentially register writes to the devices. NIC drivers
write to these registers to control the NIC for things like enabling/disabling,
starting transmission, configuring queues, etc. The driver is also responsible
for controlling where the packet is placed for receiving and transmitting. The
driver allocates packet buffers in memory, tells the NIC the address of these
buffers, and the NIC writes and reads from these buffers.

## IRQs and softIRQs
Interrupt ReQuests (IRQs) is Linux's interrupt model. The NIC "fires"
interrupts to CPUs to notify the operating system of events such as receiving a
packet. IRQs are handled by Interrupt Service Routines, functions defined by
the kernel to handle interrupts on specific devices. The problem with
interrupts is that they are blocking operations. When an interrupt is fired on
a CPU core, the core blocks until that interrupt is serviced. This is because
interrupts touch hardware and kernel state that is shared across the entire
system. Hardware interrupts by default cannot be nested in Linux because of
this. Furthermore, context switching cannot happen druing an ISR because the
kernel allocates a stack in the interrupted process's kernel stack to handle
the interrupt. If the process were to context switch during an ISR, the stack
of the ISR would also be switched out and when the ISR is resumed again, its
stack pointer is now pointing to junk and execution is corrupted. This is why
Linux splits interrupts into two halves. The top half runs in a blocking way
where it acknowledges the interrupt and does minimal work. most of the work is
offloaded to the bottom half with softIRQs or workqueues where it can be
scheduled and preempted.

SoftIRQs are a way to offload the heavy lifting part of an interrupt so that
acknowledging an interrupt (the blocking part) is light and fast. When an IRQ
arrives, the top half (ISR) acknowledges the interrupt and schedules a softIRQ.
When the ISR returns, the kernel checks for pending softIRQs and executes them
before returning to the interrupted process's context. softIRQs still run on
the interrupted process's kernel stack and cannot block. However, hardware
interrupts are enabled again and IRQs can be handled. When a packet arrives at
the NIC, the NIC DMAs the packet into the system memories DMA buffer and raises
an interrupt to the CPU. The hardware interrupt fires the ISR (top half) and a
softIRQ is scheduled. Once the ISR returns, the softIRQ runs where it starts
polling NIc devices (NAPI) for packets. The softIRQ reads the rx buffer and
passes it to the network stack.

## NIC TX and RX rings
A NIC ring is a circular array of packet descriptors located in the system
memory that both the NIC and the kernel can access. The NIC and the CPU
communicate by advancing indices in the ring so that you don't have to copy
between memory and the NIC. The ring lives in the system RAM. The kernel will
allocate DMA safe memory and provide the physical addresses to the NIC. The
rings contain descriptors which point to where the actual data is stored, how
much data is stored, and additional flags and statuses. The RX ring is used for
incoming packets. The driver allocates packet buffers and fills the RX ring
with addresses to thes buffers that the NIC can put the data into. When a
packet arrives, the NIC takes the front of the NIC and writes data into the
buffer it is pointing to. During the softIRQ for network receives, the kernel
checks the RX descriptor and if the status is done, it gives it to the network
stack. The TX ring is used for transmitting packets. When an application calls
send, the kernel builds a packet with the `sk_buff` struct. It puts the packet
in a DMA-safe buffer and adds the descriptor to that buffer in the NIC TX ring.
The NIC is continuously polling the TX rings for new entries. When it sees a
new entry, it reads the packet with DMA and sends it on the wire. The
descriptor is marked as sent and the driver will later free that buffer for
future use. Because the NIC and the kernel need to share states to make this
work, the indices of the rings are shared using MMIO registers, memory barriers
and cache-coherent DMA. Between the kernel and the NIC, there are no locks or
atomic instructions. It is a typical producer consumer queue that needs to be
synchronized. To do this, memory barriers are used. Memory barriers are
instructions that tell the processor, all previous memory writes must become
globally visible before continuing. Even if memory writes are reordered, this
ensures that the descriptor states are globally synchornized. Cache coherent
DMA is also used so that the CPU doesn't read from a stale cache line when
reading a packet. When the NIC DMA writes a packet, the cache coherence
protocol will invalidate the cache line on the CPU so that it doesn't read from
a stale line. For some architectures that don't implement coherence protocols,
the driver must manually sync caches and memory. 
