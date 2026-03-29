# Work Queues

Work queues allow kernel functions to be activated, much like deferrable
functions, and later get executed by special kernel threads called worker
threads. Work queues are primarily different in that functions in work queues
run in process context while deferrable functions (softirq) run in itnerrupt
context. A process context basically means that is a schedulable task where the
scheduler can witch to another task while it is blocking. However, in interrupt
context, which is when code is run because of a hardware interrupt or because
softirqs are enabled, it does not have its own `task_struct` and cannot be
scheduled to finish later. Thus, in interrupt context, deferrable functionsa re
not allowed to block (scheduling by calling `schedule`) would corrupt kernel
state like the kernel stack and registers. However, in process context, the
work queue functions are allowed to block because they can resume when they are
scheduled again. Because work queue functions are allowed to block, they are
the primary mechanism for Disk I/O, memory allocation, or blocking operations
triggered from interrupts.

The work queue struct contains an array of `NR_CPUS` elements, the number of
CPUs in the system, a spin lock to protect the structure, a pointer to the head
of a doubly linked list of pending functions in queue. Work queue functions are
executed by kernel worker threads called `kworker/n` that execute in kernel
mode. Worker threads for a work queue used to be one per CPU but in modern
days, they are dynamically created and shared. The kernel manages the
concurrency between the worker threads and worker threads are no longer fixed
on a CPU core unless requested. Thus there are different types of work queues.
Bounded work queues that must run on a specific set of CPUs, unbounded that can
be run anywhere.

Every worker thread continuously executes a loop, removing an element off of
the work queue list and executing the corresponding function. Most of the
times, the worker threads are sleeping but after they execute a function, they
can be scheduled off or even migrated to a different CPU while they block. The
work queue also has a doubly linked list of work functions that are done. There
is an API that allows the calling process to block until all pending functions
in the work queue list is done. However, any work added to the queue after the
calling of the blocking API call will be ignored.

While new work queues can be created, there are predefined work queues that
kernel code can use instead of creating its own. Some of these global work
queues are
- `system_wq`: General purpose default queue
- `system_highpri_wq`: High priority tasks
- `system_long_wq`: Long running tasks
- `system_unbound_wq`: No CPU affinity, can be run on any CPU
- `system_freezable_wq`: Suspended during system freeze
- `system_power_efficient_wq`: Power saving

Because shared queues are shared, long blocking tasks block the rest of the
work placed in the queues. Best practice is to use the `system_long_wq` or your
own queue if you will be blocking for a long time.
