# The Nucleus of a Multiprogramming System

## Summary

## Introduction

The nucleus for a multiprogramming system for the RC 4000 computer allows users
to extend it so that they can get the desired operational mode that they need.
In operating systems at the time, systems were designed with a single mode of
operation in mind, whether that be real time, time sharing, or batch
processing, it didn't allow users to change the mode of operation easily.

## System Nucleus
The idea behind nucleus is that instead of designing based on assumptions made
for a certain operational mode, the nucelus would provide the fundamental
features needed for parallel cooperating proceesses, these features being
- Concretely defining what a process is and being able to simulate them
- Primitives for synchronization and communication between processes
- Ways to create, control, and remove processes

## Processes
Nucelus differentiates processes as either internal or external proceses. An
internal rpocess is a an interruptable program(s) that has its own address
space and a unique process name. It is important program is not a process. A
program is a collection of instructions while a process is the execution of
these executions. Thus, you can have multiple programs in a process. External
processes are the input and output to a given a document or peripheral. A
document is a collection of data storedon a physical medium while a peripheral
device is a hardware connected to a data channel. External processes also have
a unique process name and abstracts away the device details from the internal
processes.

Communication between processes are done through an interrupt response program
that can control the input and output, storage protection, as well as the
interrupt system. The interrupt system is more so and extension of the hardware
and implements the process concept allowing communication, create, and control
of processes.

## Process Communication
This paper says that Dijkstra's sempahores approach to synchronization is not
enough because processes can be bad actors and "break the rules of the game".
Instead, it uses a buffered messages as the basic means of process
communication where the nucleus manages a pool of message buffers and queues
for each process. Internal processes can 
- send message: copies message into the first available buffer and dleivers it
in the queue of the receiver. If receiver is waiting, it is activated.
- wait message: delays requesting process until message arrives in queue
- send answer: copies answer into the buffer in which the message arrived in
and delivers it in the queue of the original sender. sender is activated
- wait answer: delays requesting process until an answer arrives in buffer

## External Processes
Communication between internal and external processes is done with send message
and wait answer. The message contains some kind of storage area and the
external process reads or writes the input or output in that storage area. When
there is an interrupt on the input/output, the nucleus gives the internal
process things like the size of block and the error conditions. External
proceses are created on request from internal processes. To make sure internal
processes have exclusive access to a resource, primitives are available for
reservation and releaes of external processes. A separate process that
interfaces with the clock is responsible for generating interrupts. An internal
process sends a messgae to the clock process and the interrupt is generated
when the clock process responds with an answer.

## Internal Processes
Internal processes are created on request from other internal processes. The
parent process selects a contiguous storage area within its own pace where the
process will be placed. After creation, the parent process can load a program
and start it. The parent process can also wait for the completion of the child
process. Parent processes can also remove child processes. The nucelus provides
the fundamental primitives needed for creating and controlling processes. The
matter of scheduling and and swapping processes is up to the user. Child
processes are coordinated by the parent processes.

## Process Hierarchy
The system starts with nucleus and a basic operating system process which can
create child processes. These child processes can also create child processes.
Every time a parent creates a child process, it is essentially the operating
system for that child process, scheduling and swapping between the children.
The processor is scheduled in a round robin fashion among all processes and any
process is able to communicate with any other process.
