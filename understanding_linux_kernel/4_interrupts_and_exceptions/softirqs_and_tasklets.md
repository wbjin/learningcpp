# Softirqs and Tasklets

Softirqs are a fixed set of kernel defined deferred execution slots. They are a
per-CPU, scheduler slot reserved for core kernel subsystems that need
deferrable work to be executed. Historically, softirqs were the core mechanism
(a way to get CPU execution time) that powered tasklets. The softirq functions
ran the tasklet functions that could be dynamically registered. In modern
Linux, tasklets are no longer used. Softirqs are the primary execution
mechanism for kernel subsystems that need deferrable work to be eecuted. Each
softirq has a global action function that executes things like handling timer
interrupts, network packet transmission (flushing transmit queues, qdisc
scheduling, pushing packets to NIC drivers), network packet receive, block I/O
callbacks, etc.

Much of the work of handling an interrupt is non-critical and can be deferred
keeping the interrupt handler small and fast. Interrupts can be put into two
categories, hard IRQs that are triggerred by hardware and soft IRQs that are
triggered by the kernel. Soft-IRQs are used to handle this deferrable work by
means of deferrable functions: softirqs, tasklets, and work queues. However,
nowadays, tasklets are discouraged. Softirqs are statically allocated and
defined at compile time. Tasklets are layered on top of softirqs and can be
dynamically initialized. Tasklets are largely legacy and have been migrated to
work queues or threaded IRQs. Softirqs can concurrently run on differnet CPU
cores, even if they are of the same type. Softirqs are reentrant functions
which means they must explicitly protect their data structures with spin locks.
A term that is widley used is interrupt context. It sepcifies that the kernel
is currently executing an interrupt handler or a defferable function. However,
nowadays, the differentiation is more granular between hard IRQs, softirq,
tasklets, workqueue, and threaded IRQ. Each deferrable function has four kinds
of operations that can be performed on it
- Initialization: Define a new deferrable function, usually done at kernel
initialization or module load
- Activation: Marks a deferrable function as pending which means to be run the
next time the kernel schedules an execution. This can be done within an
interrupt context or process context
- Masking: Selectively disables deferrable functions so that it is not executed
by the kernel
- Execution: Executes a pending deferrable function with all other pending
deferrable functions with the same type

Largely, deferrable functions raised (activated) on a CPU usually runs on the
same CPU. However, this many not always be the case because it can be processed
by `ksoftirqd` which is a per CPU kernel thread. In the modern kernel,
workqueues are the default for deferred work.

## Softirqs
Differnt softirq types have different priorities. They are numbered from 0 to N
where a lower index means higher priority. The different types are
- HI_SOFTIRQ: Handles high priority tasklets
- TIMER_SOFTIRQ: Tasklets related to timer interrupts
- NET_TX_SOFTIRQ: Transmits packets to network cards
- NET_RX_SOFTIRQ: Receives packets from network cards
- IRQ_POLL_SOFTIRQ: For blk-mq polling
- SCSI_SOFTIRQ: Post interrupt processing of SCSI commands
- TASKLET_SOFTIRQ: Handles regular tasklets
- SCHED_SOFTIRQ: Scheduler housekeeping
- HRTIMER_SOFTIRQ: For high resolution timers
- RCU_SOFTIRQ: For deferred RCU callbacks

### Data structures used for softirqs
These IRQ types are put into an array with higher priority softirqs being lower
indices. Each entry of the vetor has a struct that contains a pointer to a
function. Another important data structure is a bitfield that is part of each
process descriptor's `thread_info` field. This bitfield tracks preemption
disable depth, softirq counter, hardirq counter, and a NMI counter. The
preemption disable depth keeps track of how many times kernel preemption has
been explicitly disabled on the local CPU where 0 means kernel preemption has
not been disabled. The softirq counter specifies how many levels deep the
disabling of deferrable functions is (0 == not disabled). The hardirq counter
is the number of nested interrupt handlers on the local CPU. This bitfield is
needed for dsiabling kernel preemptability and this can be done quickly by
checking this bitfield.

Softirqs run on per-CPU IRQ stack or the `ksoftirqd` thread stack. There is
also a per CPU bit mask that describes the ending softirqs which tells the CPU
which softirqs have been activated.

## Threaded Softirqs
Threaded softirqs are a modern way of executing softirqs. It is a way to
execute softirqs in a separate per-CPU kernel thread instead of in an interrupt
context. It was introduced to offload long running softirqs to a kernel thread
so that it can be scheduled work later on instead of blocking the CPU. When
softirqs run in an interrupt context, preemption is disabled and the softirq
cannot sleep blocking everything on that CPU. By deferring it to a kernel
thread that can be scheduled, (though it still cannot block), it prevents
starvation of other tasks on the CPU. `ksoftirqd` was introduced as a kernel
thread that takes over the execution of softirqs when interrupt context softirq
execution started taking too long or if too many are pending.

### `ksoftirqd` kernel threads
The kernel creates `ksoftirqd/N` threads where N is the number of CPU cores
(logical). The thread executes a loop that puts itself to sleep until woken,
checks the pending softirqs, executes the softirqs, loops until all backlog is
drained, and calls the scehduler to let other processes run. It runs at a low
priority to prevent blocking other processes but it prevents softirq starvation
and unbounded softirq execution. The motivation of this design is to avoid the
extremetes of either continously checking for pending softirqs in the handler
which starves other tasks and not rechecking which creates additional latency
because pending softirq tasks can only be executed after another timer
interrupt. Instead, pending softirqs can be handled by the kernel thread where
if the softirq handler uses its time budget and still has pending softirqs, it
wakes up the kernel thread. In `PREEMPT_RT` kernels, softirqs are fully
threaded. In modern networking, polling, interrupt mitigation, and batching is
more frequent which means there are larger softirq bursts where `ksoftirqd`
shines.

## Extra
Softirqs are invoked when a subsystem raises a softirq. This sets the softirq
type to pending but does not actually start the softirq. When the kernel hits
specific checkpoints, it checks if there are any pending softirqs and if there
are, calls the `do_softirq()` function. This can be in places like hard IRQ
(hardware interrupt handler) exists, before the CPU enters an idle state, when
returning to user mode, during rescheduling, etc. 
