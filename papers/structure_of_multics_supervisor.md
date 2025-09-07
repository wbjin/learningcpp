# Structure of the Multics Supervisor

## Summary
The MULTICS system is built as if it was an energy or water provider. The
objectives of the system mimic that of utility providers. The most interesting
part of the paper seems to be the dynamic linking feature that allows processes
to share code without having to have its own copy at all times. Paging and
addressing seem to work like it does in modern systems with the exception of
users being able to choose their own memory regions in terms of where the heap,
stack, static data, and code lives. It also seems like the concept of processes
is similar to that of papers at the time and what my notion of what a process
is. Something interesting about the scheduling of processes however is the
assumption that the system will experience periods of low demand and high
demand at all times and usually never where the demand matches the systems
capabilities exactly. This is taken into consideration when designing the
scheduling of processes.

## Introduction

This paper is about the design of an operating system for time sharing and
multiprogramming. 

## Viewpoints and Objectives
The initial version of multics is not a finished product but a prototype for an
evolving system. Therefore, an important objective of Multics is that it should
include any features that makes it easier for future changes or extensions.
While the initial cost of doing this is substantial, it will be worth it in the
future when reowrks need to be done.

Multics is designed to be a single level system. Modules of the operating
system are indistinguishable from user programs except that they are guarded
against unintended protective locks. Multics is designed to be like a power or
water copmany:
- Utility must be more dependable than existing hardware
- Utility must provide service on demand without advance notice
- Utility must provide small amounts of service to small users and large
amounts of services to large users
- Utility must not meddle in customer's business 
- Utility charges for its service based on the amount of service
- Utility must provide service to customers who neither know or nor wish to
know the underlying mechanism

In operating systems terms, this means
- File system designed to be more dependable than the individual hardware
pieces that are used to store data
- OS dynamically adjusts to compensate for temporary loss of one or more
hardware modules
- Designed to provide service without batching or rescheduling
- Scheduling to allow small and large jobs to run together on the system
without differentiating them
- Operator does not need to actively monitor and maintain the system as long as
there are no unusual activities

The most important principle of an operating system is to be sympathetic to its
users, allow them to solve problems without having to write code. Multics
doesn't provide ways to do this nor can any programming language. The only way
to do this would be development in application level programs relavent to the
specific needs of the users. Multics provides a framework which the user can
interact with these special applications without having to worry about the
hardware

## Technical Policy for Writing Software
Absolute Mode (execution without relocation of addresses)
- First two instructions of each trap answering routine
- For startup of a cold machine
- For the initial stages of catastrophic recovery
- For appropriate product service routines

Master mode (execution with unrestricted access to privileged harware features)
- For absolute mode execution
- To exercise privileged hardware features
- Where temporary disabling of all interrupts is required
- For appropriate product service routines

## Processes
A process is the execution of a program and a program is the pattern of bits
which the hardware decodes. A process constitutes of the necessary steps for a
program to run and the program itself. For example, building the source
program, executing it, and producing output files. A program consist of the
code and data that the user intended to execute and a process consists of
things like OS metadata and state associated to run the program in a system.
Each process can be distinguished with a descriptor segment and a 645 CPU is
only able to use one descriptor segment at a time. The beginning of a process
is when the CPU loads the descriptor segment and the end is when the CPU evicts
the descriptor segment. Each process also contains a stack segment for user
programs and concealed stack segment for metdata that needs to be kept
safeguarded from user programs. Most processes are initiated by users, some
will be for system maintaince (such as purging disc).

## Status of a Process
A process is either running, ready, or blocked. A process is running if its
descriptor segment is being used as the descriptor segment of some CPU. A
process is ready if it is not running but is not held up awaiting any event. A
process is blocked. if it is awaiting an event from the external world or
another process.

## Segmentation, Paging, and Addressable Storage
Segmenting is a way to divide virtual memory into different regions for
different access privileges and uses (stack, heap, code).

Addresses are addressed to an address space instead of actualy physical
addresses in core memory. An address contains a segment number (virtual page
number) and a word number (offset). In Multics, the user can divide the program
and data into segments, choosing the segment size, names, and types of
segments (The user can parition user space however they want I guess, no
predetermined address location for stack, heap, program, data and stuff).
Paging is still an OS responsibility. The user need not worry about whether the
data is in core or disc. 

However, there are real time applications where for correct behavior, data must
be in memory at the time of an interrupt or signal. Therefore, methods are
provided for 
- bolting procedures and data into memory
- encourage data to be brought into core because it will be accessed soon
- encourage data to be removed from core because it won't be accessed

The pager operates on three sources of information. A page not in core fault
occurs and it needs to evict pages and bring in the requested data using a used
bit to tell if a page is being used and a specific location in the mapping of
the memory dedicated for the actual page table that cannot be evicted. It also
knows specific regions to not evict from.

## Segments and files
Segments are files and files are segments. A reference to one of these can be
by segment or by file. Segment referencing is the use of the two component
address (word number and segment number). File reference is different where
every file is a segment of some procedure that is responsible for that file. A
call to edit the file is a call to the procedure responsible for that file.
Every file is mmaped into memory in Multics, there are no directly read and
writes to disk

## Peripheral Devices and Files
A special file in Multics is the serial file. Unit record equipment and
typewriter consoles will be treated like serial files. A program that handles a
peripheral device as a serial file attempting to perform an illegal primitive
can be either that the primitive was performed successfully or a diagnostic
will occur

## Scheduling
Some assumptions made in Multics scheduling are
- Goal of scheduling for this type of system is to give good service to
customers at reasonable cost
- On an overloaded system, scheduling should be done to minimize overhead and
complete the most urgent work. Two ways to do this is to deny service instead
of accepting and degrading performance and to minimize switches from one
process to another
- The urgency or importance of a task is determined by humans
- An increase in overhead in an undelroaded system is acceptable if it means
improved service

The paper says that the system should face various loads at various times
(overloaded at peak hours and underloaded other times). Any system that faces
constant overload has too much work with limited hardware and a system that is
constantly underloaded has spent too much resources on hardware. Therefore, the
scheduler should be designed with these assumptions in mind and also not make
any assumptions about urgency of jobs, relying on humans for this
classification. Multics scheduling relies on an assumption that in a dynamic
storage system, excessive overhead due to accepting too many jobs is the cause
of overloads rather than the demand of the jobs themselves.

## Dynamic Linking
The linking of one procedure to another or the linking of a segment to a
procedure is done dynamically. In other systems at the time, references to
other procedures or data had to be done statically at compile or load time. In
Multics, these references are left symbolic until the first use in runtime and
only when it is accessed is the symbol resolved to actual reference the correct
segment. Code was sahred between processes but each process kept its own
connection to the code in its data segment. When a foreign function or data is
accessed, the linker wil replace these symbolic references to actual addresses
in memory. To do this, each compiled segment needed to contain the symbolic
names of external symbols and for each external reference, the name of the
target segment, the name of the symbol with the target, and a piece of data
indicating whether it needs to be loaded.

## Trap Handling
Hardware traps can be divided into two categories, process traps which is a
consequence of some instruction run by a process and system traps which may or
may not be relevant to running processes. The running process is responsible
for the process trap while it may not be for the system trap. For system traps,
when it happens, control is given to a special trap process and the appropriate
trap handler is called. The system trap needs to figure out which process was
responsbile for the trap and then bring the responsible process into the ready
state, ready to be woken up with an interrupt. Something special to Multics is
the ability for users to write their own trap handlers with varying
flexibility. 

## Creation, Blocking, and Termination of Processes
Every process is created by some other process. Certain system processes exist
just for spawning customer processes. However, any process is able to spawn
another process and specify which segment is to be shared, receive copies of,
and what the new process should essentially know and when it should start. When
a process becomes blocked, it will indicate that it is blocked with a special
flag that must be cleared when it is to be resumed again. This means that some
other process needs to clear this flag. Processes also hae a maximum time for
which it can be blocked. A special process will scan the task list for
processes which have been blocked and if one has been blocked for over the
limit, a diagnostic message is generated and shipped to the error message file
for the process. Then the blocked process is completely removed. Human
intervention is needed at this point to resume or diagnose the issue. The same
happens if a process violates some rules. 

## Protection against Machine Errors
- CPU's are run symmetrically so any CPU can be pulled at any time without
stopping the system
- Minimize absolute mode operations to minimize core banks missing
- No ways to address catastrophic CPU and drum errors
- Allowing product service while operating hard and will likely be the hardest
and last thing to be solved
