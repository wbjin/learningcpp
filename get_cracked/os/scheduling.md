# Scheduling

## Multi-Level Feedback Queue
MLFQ is a well known scheduling method that tries to balance turnaround time
(the time it takes a process to finish running) and responsiveness (time
between process executing on the CPU). These are often conflicting goals.

The basiscs of a MLFQ are:
- Multiple queues, each equeue with a distinct priority level. Queues with
higher priorities are prioritized
- Any given task is on one queue at a time
- If there are multiple tasks in a queue, round robin is used between them

They key in MLFQ is the adjustment of a task's priority based on its observed
behavior. Eg task that relinquishes CPU often and waits for I/O assigned high
prioritiy because it is likely an interactive task.

The workload that MLFQ tries to tackle is a mix of interactive tasks that are
short running and long running, CPU bound tasks. When a task is first submitted
to the system, it is placed in the highest priority. After every allotment,
some designated time that a task can spend at one priority level commonly set
to some multiple of a time slice, the priority of the task decreases by one, ie
it goes down to the queue one below the one it is in currently. If the task
gives up the CPU during its allotment, it remains in its current queue.

The key idea behind this is that MLFQ assumes a task is a short running
initially. If it is a short task, it will be prioritized and finished. If it is
not and it turns out it is a long running task that is more of a batch job, it
will be lower in priority.

MLFQ also needs a mechanism to adjust priorities upwards. This is to avoid
starvation as a large number of interactive short tasks will prevent any long
running tasks from running. Additionally, characteristics of a long running
process may change over time and it may become more interactive.

A way to combat starvation is to raise all tasks to the highest priority every
so often. A simple approach would be to do it every some time period S.

In order to avoid gaming the scheduler, we shouldn't resort to simple
heuristics such as I/O requests indicating interactiveness of a process.
Instead, we can remember the amount of allotmnet a task actually uses across
scheduling runs. Regardless of how the allotment is used, whether in one long
burst or in multiple shorter bursts with I/O in between, if a task uses up all
of its allotment its priority is decreased.

Rules of MLFQ
1. If Priority(A) > Priority(B), A runs (B doesn’t).
2. If Priority(A) = Priority(B), A & B run in round robin.
3. When a job enters the system, it is placed at the highest priority (the
topmost queue)
4. Once a job uses up its time allotment at a given level (regardless of how
many times it has given up the CPU), its priority is reduced (i.e., it moves
down one queue).
5. After some time period S, move all the jobs in the system to the topmost
queue.

## Multiprocessor Scheduling
Multicore processes need cache coherence across caches in different cores. MESI
is a protocol that ensures all CPU cores sees a consistent view of memory. MESI
has three states for each cache line
- Modified: Only in this core and is dirty
- Exclusive: Only in this core and is not dirty
- Shared: In multiple cores and is not dirty
- Invalid: Invalid to read or write to

The core mechanism of MESI lies in bus snooping. Cores are connected by a bus
on which they send messages such as "read X", "write X", and "invalidate X".
For example, consider the scenario where two cores share the cache line X. (A
cache line is the smallest unit of transfer between memory and CPU caches and
is typically 64 bytes)
1. Core 1 reads X. The state is E
2. Core 2 reads X. The state in core 1 and 2 are S.
3. Core 1 writes to X. Core 1 tells other cores to invalidate its copy. Core
2's X is I. Core 1's X is M
4. Core 2 reads X. It is I so it issues a read request on the bus. Core 1 sees
this and its state is M so it either does a cache to cache transfer or
writes back to memory.

## Linux CFS
The goal of CFS is to fairly divide the CPU among all processes competing for
it. The key mechanism it does this by is with the concept of virtual runtime,
`vruntime`. When a task runs on a processor, it accumulates `vruntime`,
typically in proportion with real time. When a scheduling decision needs to be
made, the task with the lowest `vruntime` is chosen.

A key design choice with CFS is determining when to preempt a process to make a
scheduling decision. Making scheduling decisions often will be more fair but at
the cost of performance due to switching overhead. Making scheduling decisions
less often will lead to better performance but less fairness over the short
term. Linux exposes this control knob through the `sched_latency` parameter.
`sched_latency` is divided by the number of processes in a system to determine
the `vruntime` a process gets before the next scheduling decision.
`sched_min_granularity_ns` is the absolute minimum time slice.

CFS takes process priority hints from the user through niceness which is a
score assigned to a process that takes an integer value between -20 to 19
(negative is higher priority). Linux assigns weights to these niceness scores
```C
static const int prio_to_weight[40] = {
/* -20 */ 88761, 71755, 56483, 46273, 36291,
/* -15 */ 29154, 23254, 18705, 14949, 11916,
/* -10 */ 9548, 7620, 6100, 4904, 3906,
/* -5 */ 3121, 2501, 1991, 1586, 1277,
/* 0 */ 1024, 820, 655, 526, 423,
/* 5 */ 335, 272, 215, 172, 137,
/* 10 */ 110, 87, 70, 56, 45,
/* 15 */ 36, 29, 23, 18, 15,
};
```
and it uses to compute the time slice for a task using the following formula
$$
\text{time\_slice}_k = \frac{\text{weight}_k}{\sum_{i=0}^{n-1} \text{weight}_i} \cdot \text{sched\_latency}
$$

The `vruntime` accumulation also uses this weight
$$
\text{vruntime}_i = \text{vruntime}_i + \frac{\text{weight}_0}{\text{weight}_i} \cdot \text{runtime}_i
$$

CFS keeps tasks in a red black tree (a kind of balanced binary search tree) for
ensured logarithmic search, insertion, and deletion times. The red black tree
only contains runnable tasks and not blocked tasks.

When a blocked task wakes up, simply scheduling it would make it dominate the
procesor for a while (because its `vruntime` will be much smaller than the
other tasks). Instead, CFS assigns the min `vruntime` found among the tasks to
the task that becomes unblocked.
