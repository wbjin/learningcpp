# IRQ Trace

This is the analysis of the function graph generated with the IRQ.

## Hardware interrupts and IDT
The kernel maintains an Interrupt Descriptor Table (IDT) that contain pointers
to interrupt ahndlers. The NIC will raise an interrupt on the interrupt line,
the Advanced Programmable Interrupt Controller (APIC) will translate that
interrupt to a vector number and which CPU to run the handler on. Modern NICs
skip the I/O APIC. Instead they use MSI/MSI-X bypass where it does a DMA write
to a memory ammped address by the local APIC. The local APIC sees this and
raises an interrupt on the CPU. The flow looks kind of like
1. NIC raises an interrupt with LAPIC MMIO address
2. Target CPUs LAPIC sees the interrupt and enters the interrupt handler
3. The interrupt handler will clear the LAPIC MMIO address so further
   interrupts can be handled

`handle_irq_event`: Entrypoint into the interrupt handler. Dispatches to the
corresponding ISR associated with the IRQ vector. 

`igb_msix_ring`: Acknowledges the IRQ, schedules napi

`napi_schedule`: Schedule NAPI polling as softirq

`net_rx_action`: The entrypoint into bottom half ISRs. Invoked via softirq.

`napi_poll`: Dispatch to the corresponding device drivers polling
functionality.

`igb_clean_tx_irq`: Reclaim sk_buff structure for transmitted packets, clear
corresponding ring descriptors

`igb_clean_rx_irq`: The main driver of the receive functionality. Runs XDP
operations if enabled. XDP (eXpress Data Path) is packet processing at the NIC
level using eBPF call backs before even the `sk_buff` is allocated. Constructs
the `sk_buff` associated with the received packet. There are ways the `sk_buff`
can be initialized, in the headroom of the DMAd memory region that the driver
allocatedwhich is fast because there is no separate allocation and memcpying of
the header. Another is to allocate and memcpy the headers separately from the
DMAd region. Hands off to the generic network stack called the GRO (Generic
Receive Offload). By the end of this function, the packet is in system memory
with the corresponding `sk_buff` constructed, and GRO coalescing is attempted.

The network drivers process of telling the NIC where to write the packet is
very similar to the send path.

1. Driver maps packet data to DMA so NIC can see it.
2. Driver writes the address of the data into the ring descriptor
3. The device driver increments its next index pointer
4. The device driver writes the new tail index to the NIC register to indicate
   there is one more packet. This is done through PCIe MMIO
5. NIC reads the RX ring via DMA
6. NIC finds new descriptors and DMA writes the packet into the buffers

`napi_gro_receive`, `dev_gro_receive`, `inet_gro_receive`, `udp_gro_receive`:
Coalesce packets as much as possible, essentially patching the packets with
same flow, combining two `sk_buff` into one etc. This saves on stack traversal
cost, scheduling/wake up over head for `sk_data_ready` which checks whether the
application should be awaken, `copy_to_user` efficiency where bigger chunks of
data can be copied to the user program.

`napi_complete_done`: Entrypoint into the kernel network stack to process the
packets.

`ip_rcv_core`: IP header validation.

`ip_rcv_finish_core`: Where the routing decision is made. For an end host, the
packet takes the `ip_local_delivery` path if it matches if the IP address
belongs to one of the network interfaces on the machine.

`ip_sublist_rcv_finish`: Delivery to either local delivery or forwarding for
routers.

`ip_local_deliver_finish`: Dispatch into transport layer protocol.

`udp_rcv`: Socket lookup and delivery. It looks for the socket bound to the
specific IP address and port.

`udp_unicast_rcv_skb`: Deliver the packet to the socket. 

`__udp_enqueue_schedule_skb`: The `sk_buff` structure is now owned by the
socket's receive queue. It still sits in kernel memory where it was DMAd for
NAPI but the driver no longer has a reference to it. By the time all processing
is finished, the `skb->data` is pushed past all the headers and now points at
data. After this returns, the packet is in the `sk_buff` slab area where small
packets are copied into the `sk_buff` object itself. Large packet data is
stored in the DMAd page fragments.
