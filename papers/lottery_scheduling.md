# Lottery Scheduling: Flexible Proportional-Share Resource Management

## Summary
This paper introduces lottery scheduling, a probabilistic and randomized
resource allocation mechanism. The idea is to achieve fairnes and no starvation
while simplifying resource allocation.

## Introduction
Scheduling in multithreaded systems is challenging because there are many tasks
of variying importance and limited resources. Many applications require fine
grained control over the computation time allocated for that application for
predictable quality of service. Conventional schedulers don't achieve allow
this because they rely on ad-hoc and poorl understood priority schemes.
Schedulers that do aim to do this like fair share schedulers and auction based
schedulers can achieve this to some extent but is still fairly coarse and has
high overhead. The authors' proposed lottery scheduling achieves proportional
execution rates and dynamic responsiveness that has low overhead. The authors'
implement in the Mach 3.0 microkernel.

## Lottery Scheduling
The key idea is to assign each client lottery ticks where more lottery tickets
means a higher change for resource access. Tickets are not limited to a single
resource and can be applied to things like procssor and I/O. This scheme
achieves fairness through probability where the more times a client
participates in a lottery, the more likely they are to win a resource where
this probability follows a binomial distribution $n * t/T$ where n is the
number of lotteries, t is the number of tickets a client has, and T is the
total number of tickets. The time it takes for a client to win its first ticket
follows a geometric distribution of 1/p where p = t/T. This approach avoids
starvation because any client with a ticket will eventually win. It is also
very responsive because ticket distribution change will take into effect the
next lottery.

## Modular Resource Management
This section describes how a lottery system allows independent modules
(applications, threads, etc.) to manage their own resources without intefering
with others. This is done through the following features on tickets
- Ticket Transfer: Clients can temporarily transfer their tickets. This allows
threads actively doing work to hold more tickets and get more resources
- Ticket inflation: Trusted clients can create more tickets to increase their
resource share
- Ticket currencies: Each group (a user or application) is allowed to have
their own ticket currencies that are backed by the base ticket. This
encapsulates inflation locally
- Compensation tickets: If a client only uses a fraction f of their allocated
resource time, it receives a bonus ticket 1/f. This ensures workloads bounded
by I/O don't get penalized for not fully using the CPU

This modular resource management allows pluggable policies at the system or
user level.

## Implementation
The lottery system was implemented in Mach with scheduling happening every 100
milliseconds. Randomization was implemented using the Park-Miller standard for
pseudo-random. The lottery algorithm was generating a random number between the
ticket ranges and traversing list of clients until a winner was found. This was
improved using a move to front heuristic where frequent winners were moved to
the front of the list and sorting the clients but ticket number. The kernel
objects for implementing this were tickets that contained a value and a
currency and currencies that contained the issued ticket, backing tickets, and
active ticket totals. The APIs that were exposed was to create and destroy
tickets and currencies, fund/unfund currencies, and computing ticket values in
base units. Conversion between currencies were based on the active ticket count
and the backing tickets. When a thread blocked, its tickets were deactivated
and activated again when the thread started running agian. Ticket transfers
done with RPCs and integrated into Mach's mach_msg system call. The ticket is
then destroyed after a reply from the receiver.

## Experiments
The authors evaluated lottery scheduling in terms of fairness, responsiveness,
flexibility, efficiency, and isolation
- Fairness: The authors ran two compute bound tasks with varying ticket ratios.
They observed that execution ratios matched that of ticket ratios. This showed
that lottery provided accurate proportional allocation especially over long
time windows
- Flexible control: The authors ran Monte Carlo simulations with ticket
inflation based on error. The authors showed that tasks convered to equal
accuracy 
- Clienet server computation: The authors ran a multithreaded server for
searching through text with automated ticket transfer. The server processed
client requests proportional to ticket allocations
- Multiemedia: The authors ran three video viewers and changed ticket
allocations mid run. The frame rates ended up tracking the ticket allocation
- Load insulation: The authors ran two groups (A and B) with equal funding. A
new group B3 joined B and only B1 and B2 slowed down while A stayed the same
- Overhead: Microbenchmarks showed that there was very little performance
penalty 

## Managing Diverse Resources
The authors demonstarte in this section that lottery scheduling is also
applicable to synchronization primitives, I/O, memory, and network
communication.
- Synchronization primitives: The authors introduced a lottery scheduled mutex
where all threads waiting for a lock transferred their tickets to a lock
currency where the mutex holder inherits these tickets to ensure it finishes
faster.
- Memory: The authors introduced the concept of inverse lotterries where
instead of choosing a winner, you inverse the lottery to choose a loser that
gives up a memory page.
- Multiple resource types: For managing multiple resource types, you could
assign a manager thread to each application that adjusts tickets for CPU and
memory
