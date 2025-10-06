# DistServe: Disaggregating Prefill and Decoding for Goodput Optimized Large Language Model Serving

## Summary
LLM serving traditionally does prefill (generating the first token for the user
input) and decoding (generating the subsequence tokens) on the same GPU.
DistServe proposes disaggregating these phases to separate GPUs and optimizing
them individually for maximizing the number of requests served within some time.

## Introduction
LLM inference is done in two phases. The prefill phase processes a user's input
prompt in one step. This is followed by the decode phase output tokens are
generate in multiple steps. This is the key different in LLM inference
workloads. An LLM servie measured with two key metrics: time to first token
(TTFT) which is how long the prefill phase takes and time per output token
(TPOT) which is the average time taken to generate an output token. Services
like chatbots have different priorities depending on the nature of the request.
For short, real time requests, you want low TTPT for responsiveness and a TPOT
that matches the human reading speed. For tasks like summarization, low TPOT is
ideal for faster output generation. Thus an LLM inference system need to
balance these requirements and maiximize per-GPU goodput which is defined as
the maximum request rate that can be served adhering to the SLO goal for each
GPU used.

The prefill and decoding phases share the same LLM weights and working memory
so they are typically processed on the same GPU to maximize overall system
throughput. However, to meet latency requirements, these systems that colocate
phases must overprovision compute. Simply separating the two phases in
different GPUs would lead to much lower TTFT and TPOT. This is because the two
phases have drastically different computation characteristics and latency
requirements. Doing both prefill and decode in the same batch increases both
TTFT and TPOT. Scheduling them separately in different batches doesn't solve
the issue as well because of queueing delays where prefills have to wait for
decodes and decodes have to wait for prefills. The two also have diffeent
latency requirements and have preference for different forms of parallelism.
Colocating couples their resource allocation and prevents the implementation of
parallelism more suited to each phase.

To overcome these challenges, the authors propose to separate prefill and
decoding to separate GPUs. This eliminates the interference between prefill and
decode and it also allows each phase to scale independently with tailored
optimizations and parallelism strategies. They build DistServe, an inference
system that optimizes for goodput that can individually scale the prefill and
decode phases but also scale the entire sytem to meet user required traffice.

The novel ideas from this paper are:
- Identifying the problems of prefill-decoding interference  and proposing to
disaggregate the two phases
- Designing a placement algorithm to choose a schema that optimizes for
goodput. Essentially designing the strategy to maximize prefill and decodee
without too much overhead
- Evaluate DistServe

## Background and Motivation
### LLM Inference
LLM inference involves computing hidden representation for each token in a
sequence. These hidden representations can be computed in parallel for a given
sequenece. There is also substantial I/O demand as LLM weights and intermediate
activations need to be moved from memory to SRAM. The prefill phase can be
computed in one step because all of the tokens in the initial sequence is given
by the input. However, the decode stage must happen one output token at a time.
This means that not breif prefill phases are typically compute-bound while
decode phases even though they process one output token at a time still incur
the same I/O and is thus memory bound. Both phases generate intermediate states
known as KV caches that is needed for later decoding steps. The shared use of
LLM weights and KV caches is why existing systems prefer to colocate prefill
and decode.

### LLM Serving Optimization
Existing serving systems utilize continuous batching. This is where new
prefills and existing decodes are batched together. This boosts GPU utilization
and maximizes overall system throughput which is all tokens per second.
However, batching prefill and decoding invariantly leads to tradeoffs between
TTFT and TPOT which means we don't meet our SLOs. Existing systems also use
model parallelism with intra and inter-operator parallelism (TP and PP). Intra
is where computationally expensive operators are split across multiple GPUs
accelerating computation but causing substatntial communication. It can reduce
latency for TTFT but requires high bandwidth communication like NVLINK. Inter
is where LLM layers are organized into stages running on different GPUs. This
forms a pipeline where while one GPU is computing for one layer, another layer
GPU is computing for another layer. This increases latnecy due to inter stage
communication but it linearly scales the system's rate capacity with each added
GPU. 

### Problems and Optimizations
The problems with existing systems is that adding even a single prefil job in a
batch of decodes significantly reduces both. One attempt at solving this is
chunked prefills where long prefills are split into smaller chunks that
piggyback onto decode jobs. While this does mitigate the problem of slower
decodes, it doesn't solve the fundamental issue and it also introduces
additional overhead for prefill jobs as the KV cache f the previous chunks have
to be loaded when computing each subsequent chunk.

Simply scheduling prefills and decodes in different batches also doesn't solve
the issue because decoding jobs end up facing longer queueing delays as it is
dependent on prefill jobs. Additionally jobs with just decodes often lead to
GPU underutilization. Prioritizing either still ends up adversely affecting the
other.

Colocating prefill and decode also lead to the coupling of parallelism
strategies and resource coupling. Prefill typically benefit from TP while
decode usually benefits from PP and a batch size that is different from the
optimal batch size for prefill jobs.

### Opportunities
These challenges present a set of opportunities for improvement. By
disaggregating infernece to prefill and decode instances, you can decouple
resource and parallelism strategies while also scaling each phase differently.
For example, because decode usually has less GPU utilization, you can have more
prefill instances than decode instances to make sure the decode instances stay
fully utilized.

## Tradeoff Analysis

### Analysis for Prefill Instance
Prefill phase is compute intensive and can quickly reach a GPU's compute
capabilities without much batching. After reaching the GPU's compute, any more
requests to the batch only increases the processing time for the batch. The
authors denote a prompts critical input length threshold as $L_m$ where beyond
this point, batching only increases the prefill's latency and batching should
only be considered if a prompt is less than $L_m$.

In order to understand the benefits of separating the two phases, the paper
analyzes each phase and their characteristics.

The authors also analyze different parallelism strategies with prefill. The
idea is that different parallelism strategies are effective for different
situations where if your service has many users, PP allows lower queuing delay
and is more scalable. If you have less users or if you have stricter TTFT
requirements, TP is better as it reduces execution time more than PP.

### Analysis for Decoding Instance
Decoding phase is memory bound due to the load and stores of KV cache. This
means that batching is necessary to have high GPU utilization. Systems that
colocate prefill and decode make increasing the batch size difficult because it
makes it harder to meet the latency goals of TTFT. Thus, it creates a tradeoff
between TTFT and TPOT.

Even after disaggregation, the batch size of decode is limited by GPU memory
capacity and the need to maintain the KV caches for all active requests. This
requires parallelism and memory management techniques like Paged Attention. TP
can be applied to decoding as well where increasing the number of GPUs reduces
latency with diminishing returns as you increase the number of GPUs involved
because of the communication overhead. PP can linearly scale throughput so the
authors come to conclusion that if the TPOT SLO is important, TP is essential
but PP is preferable to scale throughput linearly.

### Practical Problems
Disaggregating prefill and decode has non-trivial challenges involved.

Variable prefill length: Prefill lengths in real life applications of LLMs vary
which causes pipeline bubbles during PP as longer length prefills have more
compute demand and execution time. To address this problem, the authors
developed an algorithm that searches for the best parallelism strategy based on
the workloads and schedules to minimize bubbles.

Communication overhead: Trasnferring KV caches from prefill to decode can
incure notable overheads. The KV cache of a single 512 token request on a 66B
model is 1.13 GB. Modern GPU clusters have infiniband with high throughput (800
Gbps) but in cases where this isn't available, DistServe places prefill and
decode instances in the same cluster and relies on NVLINK.

## Method
DistServe addresses the challenges above by 
- Discovering the best parallelism strategies for prefill and decode separately
- The number and placement of the prefill and decoding instances 

The authors present two placement algorithms, one for clusters with high speed
cross node networks and one for clusters without such infrastructure. The
authors also devise an online scheduling algorithm that adapts to real world
workloads.

### Placement for High Node-Affinity Cluster
For clusters with Infiniband, KV cache transmission overhead across nodes is
negligible. This means prefill and decode instances can be placed across any
two nodes without constraints. The placement in this case is to optimize
prefill and decode separately and then to replicate the instances to match the
traffic rate. That being said, finding the optimal configuration is non-trivial
as it you can't simply calculate SLO attainment as there are many variables
like input length, output length, arrival pattern, etc. To address this, the
authors build a simulator to estimate SLO given the arrival rate of requests,
and the input and output length distributions. This only works under the
assumption that historical workload patterns will continue in the future. While
it cannot predict short term intervals, the workload pattern over longer
timescales is more predictable. Given this simulator, the algorithm is to
enumerate all feasible parallel configurations given the cluster capacity limit
for both prefill and decoding instances. Then for each of thes configurations,
the simulator finds the maximum goodput via a binary search for both prefill
and decode configurations. This configuration is then replicated as many times
as needed to meet the request rate. The complexity of this algorithm is
$O(NM^2)$ where $N$ is the node limit per instance and $M$ is the number of
GPUs per node (typically 8)

### Simulator
The simulator was built by analyizing the FLOPs and memory accessses for
prefill and decoding phases. THen a latency model was used to approximate the
inference execution time.

### Placement for Low Node-Affinity Cluster
The naive palcement for clusters without Infiniband would be to place prefill
and decoding instances on the same node to utilize NVLINK. However, for large
models like 175B parameters, it may not even be possible for a single pair of
prefill and decode to be on the same node. This makes the placement for low
node affinity clusters harder. The key insight for this is that KV cache
transfer onlly occurs between corresponding layers of prefill and decode. This
means that only the matching layers between the prefill and decode have to
share the KV data. With PP, the layers of a model is grouped into stages and
stages are split across GPUs. Each stage becomes and instance segment and the
idea is that only the corresponding decode and prefill stages have to be
colocated. This makes it so that KV cache transfer only happens via NVLINK
within a node. Thus, the placement algorithm for Low Node-Affinity clusters
enumerates the different number of PP stages and TP assignments to GPUs, then
gets the intra node configs given the PP and TP dimension enumeration, and for
each config simulates the goodput and chooses the configuration with the
highest.

### Online Scheduling
DistServe uses a first come first serve scheduling policy. Requests arrive at a
central controller and is then dispatched to prefill inseteances with the
shortest queue for prefills. The scheduling incorporates these enhancements
- Reducing pipeline bubbles: Requests are scheduled in a way that balances
execution time across all batches in the pipeline. For prefill instances, the
model and GPU is profiled to find the shortest prompt length $L_m$ needed to
saturate the GPU and prefill batches are scheduled with a total sequence length
close to $L_m$ by batching shorter requests together or just having requests
that are longer than $L_m$ in the batch. For decode instances, $L_m$ is the
largest batch size.
- Combat burstiness: Bursty workloads can cause memory overload on the decode
instances as prefill KV cache is transferred. To avoid this, decode instances
"pull" KV caches, fetching them as they are needed.
- Replanning: DistServe has periodic replanning where a workload profiler
monitors average input and output length of requests, arrival rate, etc and
detects if there is a significant shift in these patterns. This triggers a
rerun of the placement algorithm
- Preemption and fault tolerance: While DistServe does not implement things
like preemption and fault toleration, it can be employed together with
disaggregation. FCFS scheduling can cause of head of line blocking and
preemptions allow blocked requests to finish earlier. Fault tolerance is not
implemented in the paper. This is especially important because disaggregation
introduces the risk of fault propagation where a fault in a decode instane that
multiple prefill instances are mapped to can cripple the entire service

## Implementation
DistServe is implemented as a end to end serving system with
- RESTful API frontend
- Placement algorithm module
- Orchestration layer
- Parallel exectuion engine

The simulator algorithm is implemented in the placement module. The
orchestration layer manages prefill and decode instances for request
dispatching, KV cache transmission, and result delivery. NCCL is used for cross
node GPU communication and async CudaMemcpy is used for intra node
communication. Each instance is managed by a parallel execution engine which
uses Ray to implement GPU workers that execute the inference. It implements
recent LLM optimizations like continuous batching, FlashAttention,
PagedAttention.

## Evaluation

## Discussion
The limitations of DistServe is throughput optimized scenarios such as offline
or batch jobs that don't care about latency. For resource constrained
environments with fewer GPUs, harder to implement.
