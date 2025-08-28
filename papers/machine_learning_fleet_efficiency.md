# Machine Learning Fleet Efficiency: Analyzing and Optimizing Large-Scale TPU Systems with ML Productivity Goodput

## Summary

ML workloads have unique performance characteristics different from traditional
high performance computing. This paper introduces a way of analyzing an
infrastructures efficiency under ML workloads and different ways to address
bottlenecks this new framework for efficiency exposes.

## Introduction
ML models have different workloads thus different hardware to address these
workloads. One such different is the rise of domain specific accelerators such
as GPUs and TPUs. Previous papers addressed the individual hardware
architecture design and performance. This paper addresses the entire ML stack
including the hardware, scheduler, compiler, the framework, and the model. The
main contributions of this paper are 1) a methodology to understand the ML
stack in a moduler way by separating it into layers 2) a way to view the
infrastructure holistically with the ML Productivity Goodput metric 3) ways to
address bottlenecks identified with the frameworks from 1 and 2

## Background
Because of the unique workload of ML models, a specialized compute
infrastructure dubbed "ML fleet" was introduced. An ML Fleet combines aspects
of Warehouse Storage Computers and high performance computing data centers.
WSCs traditionally have internet services as its workload. This means it has to
handle diverse, highly concurent, bursty, small independent tasks. High
performance computing on the other hand is focused on a small set of complex,
highly coupled computations that last long periods of time. ML fleets need a
combination of the parallelism in WSCs and compute capability of HPC. ML
workloads involve computationally expensive matrix and vector operations that
need to run parallely. ML workloads tend to also be communication bound,
meaning nodes have to communicate amongst each other very often because models
are often sharded across many chips.  ML workloads can change rapidly with new
model architectures.

The hardware used for ML fleets also vary. WSCs use general use commodity
hardware and HPCs use special acclerators for specific computations. ML fleets
on the other hand heavily rely on accelerators such as GPUs and TPUs as well as
ML workload optimized memory and SSD storage. 

This entire section contains more stuff about how ML workloads, thus ML fleets,
differ from traditional approaches to infrastructure design. ML fleets require
high bandwidth networks for the rapid communication between nodes when running
distributed training while WSCs network needs are more for handling varied
traffic globally and HPC optimize for low latency networks for tightly coupled
complex computations.

## Anatomy of an ML Fleet
This section details the different layers of the ML stack.

Accelerators are specialized hardware such as GPUs and TPUs. ML fleets are
unique in that they are centered around these accelerators while other
infrastructures are centered around general purpose or specialized CPUs. There
is much diversity amongst accelerators with various versions from various
vendors.

Scheduler directly manages the hardware by coordination the allocation of
resources ie assign the hardware to do specific tasks. The scheduler must
optimize performance across various hardware types and balance utilization with
stability and fault tolerance. This essentially means that the scheduler needs
to handle various hardware capabilities and make sure no one hardware component
is running overtime while another is idling. There is also the added element of
the constantly changing ML workload. Even in a short period of 1 year, Google's
ML fleet workload changed where the portion of "extremelt large" and "small"
workloads increased (ie training and testing in small sizes to deploying).
Schedulers also have to pay attention to not overclock hadware because that can
lead to early failure. In the case of hardware failure, the scheduler also has
to be able to handle and recover.

Runtime/Compiler bridge the gap between the ML models and the hardware the
models are trainied/inferred on. The runtime executes the ML programs by
setting up program data, feeding data, checkpointing, and managing the outputs
of the programs. It can also distribute the workload in an efficientw ay. The
compiler turns higher level code into executable code that have general and
platform specific optimizations. 

Framework interfaces between the ML application code and the runtime eg
TensorFlow, JAX, PyTorch. It provides high level abstractions and APIs to train
and deploy models and transforms user code into representations that can be
understood by the compiler and the runtime. 

ML Model and Data sit at the top of the stack and essentially dictate what is
needed underneath it. Different workloads will require different layers
underneath it such as the use of different accelerators or the use of a
different framework. 

## ML Productivity Goodput
MPG is capable of 1) capturing dynamic nature of ML fleets which change due to
different workloads, updates to code, evolving hardware 2) explain the trade
offs between individual and aggregate efficiency

The Iron Law of Processor Performance breaks down CPU performance into
$\frac{instructions}{program} * \frac{cycles}{instruction} *
\frac{time}{cycle}$. Similarly, MPG breaks down fleet efficiency to the
scheduling, runtime, and program components.

> MPG = Scheudling Goodput * Runtime Goodput * Program Goodput =
> $\frac{all-allocated}{capacity} * \frac{productive}{all-allocated} *
> \frac{predicted}{productive}$

Scheduling Goodput: How often does an application have all necessary resources
to make progress? SG quantifies how well resources are allocated within the
fleet. Some jobs need a specific set of resources for running a task and thus
needs to wait until these resources become available. The numerator is the time
where all resources needed to make progress is available and the denominator is
the time the fleet is active. 

Runtime Goodput: Of the time that an application has all necessary resources,
how often is it making progress? RG measures how much time the ML application
is spending doing productive work. The runtime has to spend time doing things
like initializing chips ,connecting them, loading and compiling programs, and
feeding data into programs during which there is no actual progress made with
training or inference. The numerator is the productive chip time that the
application spends making progress and has saved, ie even if the application
fails the intermediate results are still available. The denominator is the
allocated chip time which is the time the application spends doing something.

Program Goodput: Of the time that an application is making progress, how close
is it to the ideal roofline? PG measures the efficiency of the application code
and how well it is utilizing the resources. The numerator, is the prediction is
calculated by considering the intrinsic properties of the ML model by
analyizing the shape of the unoptimized high level operations graph and
estimating who many FLOPs the program would require at its theoretical peak
performance. The denominator is the actual execution time of the application.

## Improving Fleetwide Efficiency
SG Optimizations: Scheduling optimally is a hard problem because tasks have
various resource requirements as well as TPU topology size and the topology
itself (it is the bin packing problem which is NP hard). In Google's case, they
measured that the SG for extremely large jobs and small jobs were the highest.
In the case of extremely large jobs, the schedule was unlikely to preempt it
because the startup over head is large as well as the failures that it causes
in hardware. In the case of small jobs, they usually don't get preempted
because they are likley to finish quickly and even if it is preempted, it is
relatively easier to secure the resources for it agian. In all, the preemption
strategy is what dictates SG where you could configure to improve the goodput
of medium and large jobs at the cost of small and extremely large jobs.

RG Optimizations: The biggest problem with the runtime is host overhead and
preemptions. Host overhead can be things like transforming input data which for
large amounts of data, can be a bottleneck. In order to address these
overheads, the runtime can use techniques such as sharding the dataflow graphs
to more manageable independent pieces, asynchronously checkpointing instead of
pausing accelerators to checkpoint, ahead of time copmilation using less
expensive hardware to allow accelerators to run more important computations.
Another way RG can vary is the workload itself. Training workloads have more
constant computation demands so resources can be kept busy while servingin can
depend on user demand that fluctuates

PG Optimizations: Various techniques can be used to optimize the performance of
the ML application. You can overlap the communication and computation needs of
a model where you decompose teh communication that you need to do between chips
and the computation that needs to be done before the communication into more
finer grain operations. You can also have compiler autotuning. 

