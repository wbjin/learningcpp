# The Structure of the "THE"-Multiprogramming System

## Summary
The paper begins with Dijkstra complaining about the lack of resources compare
to the challenge of designing a multiprogramming system. The paper though in
large is about THE and its features, mainly a paged memory system, proces
scheduling and preempting, and the abstraction each layer of the system
provides.

## Tool and Goal
The system Dijkstra was building was something that would be able to process a
continuous flow of user programs for the University he was working at. The
multiprogramming system had the objectives of 1) reduction of turn around time
for programs of short duration ie fast switching between user programs 2)
economic use of peripheral devices 3) automatic control of backing store to be
combined with economic use of central processor (I'm not sure what backing
store means) 4) economic feasability to use the machine for those applications
for which only the flexbility of a general purpose computer is needed but not
the capacity nor processing power (I think this means supporting user programs
that don't need much processing at reasonable costs)

## Progress Report
Some mistakes Dijkstra's talks about is
- Taking too long to get to a "perfect installation" and having too much
complexity
- Not debugging?

Dijkstras says that because of his deep fear for malfunctioning machines and
interrupts, the system they designed was "inspired by fear". He says that it is
possible to design a multiprogramming system in a way where the logical
soundness can be proved and its implementation can have exhaustive testing. He
talks about how "the resulting system is guaranteed to be flawless" and that he
is not worried about system failure een due to unlikely situations.

## Survey of System Structure
Storage Allocation: In other systems, secondary storage was addressable with
the "drum address" and memory was simply a way to access it in a "page-wise"
format. I think this means memory simply acted as an extension of secondary
storage with the same addressing scheme for both storages. THE used a different
approach where each memory unit, called a page, could be in the core page
(memory) or the drump page(secondary) and could fit a segment exactly. The
addressable range of a segment was much larger than the available primary and
secondar memory (this is definitionally talking about paged memory) where each
segment stored a segment variable that denoted which page it could be found in.
Segments in a core page can also be evicted to a drum page (disk) where there
is no need to evict it to the same drum page from where it originally came
from. This also made it possible so that a program didn't have to occupy
consecutive drum pages, they could occupy drum pages in different areas in
secondary and primary memory that was not contiguous.

Processor Allocation: The key idea with processes in THE was that only the
sequential execution matteredand not the actual speed the instructions were
executed. This mean that the system could be build as a "society of sequential
processes" not necessarily progressing at the same rate. These processors
cooperate with "explicit mutual synchronization" (semaphores where the
underlying value exists in an address space accessible by both processes). The
underlying idea is that stopping a process from execution will never harm the
logic of the process because it is not dependent on the rate at which
instructions are executed. You can also have more processes than the processor
can handle because you can schedule processes to not be running on any
processor.

System Hierarchy: The system has levels of hiearchy.
- Level 0: Responsibility for processor allocation through the mutual
synchronization primitive. Interrupts also makes sure no process is dominating
the processor. There is also proritization of some processes for quick response
where there is needed. This level provides the abstraction of a process having
its own dedicated processor when in fact it is shared.
- Level 1: Segment controller, a process synchronize with the drum interrupt
and the processes on higher levels to maintain state with secondary storage.
Provides abstraction that memory is in the form of segments and not primary or
secondary.
- Level 2: Message interpreter getting input from console keyboard and allowing
communication between an operator and a process. Processor can display things
on the output printer and the operator can converse with a processor by
identifying which processor they want to communicate with. Hence, the message
interpreter process is needed to interpret this process identification.
- Level 3: I/O devices managed by equential processes associated with buffering
and unbuffering of input and output streams. Provides the abstraction to talk
to the peripherals as logical communication units. 
- Level 4: User program
