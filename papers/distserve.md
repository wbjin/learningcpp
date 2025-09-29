# DistServe: Disaggregating Prefill and Decoding for Goodput Optimized Large Language Model Serving

## Summary
LLM serving traditionally does prefill (generating the first token for the user
input) and decoding (generating the subsequence tokens) on the same GPU.
DistServe proposes disaggregating these phases to separate GPUs and optimizing
them individually for maximizing the number of requests served within some time.

## Introduction
LLM inference has a prefill and decoding stage that have different optimization
targets. Time to first token is important for prefill because for applications
like chatbots, users don't want to wait a long time to see results of their
prompt. Time per output token is important for the entire prompt to finish
quickly. Existing systems colocate both stages on the same GPU making it harder
to optimize for both simultaneously. DistServe disaggregates prefill and
decoding to separate GPUs which enables independent reosurce allocation and
parallelism for each phase and optimizations for TTFT and TPOT.

## Background and Motivation
LLM inference serving has several Serivce Level Objectives, these being TTFT
for responsiveness and TPOT for smoothness of output. The objective is to meet
these requirements while maximizing the number of requests per second. LLM
inference has two phases, prefill and decoding. Prefill is compute bound while
decoding is memory bound. Both phases need a KV Cache that occupies GPU memory
which means these different phases have resource contention. The current
optimization technique is to have model parallelism with intra-op where
operations like matmul is split across GPUs and inter-op where model layers are
across multiple GPUs. This causes problems where decoding is delayed by long
running prefill jobs. There are scheduling conflicts where prioritizing one
harms the other. Often times, in order to meet the TTFT and TPOT needs, extra
hardware resources are needed. Disaggregating these phases would allow
optimizations specific to each phase.

## Tradeoff and Analysis
In order to understand the benefits of separating the two phases, the paper
analyzes each phase and their characteristics.

Prefill phase: The prefill phase processes the entire prompt in parallel where
this is espeically compute intensive for long prompts with this length being
denoted as Lm. For lengths longer than this, batching does not help and is only
useful for shorter prompts. In practice, prefill batches are small. The
different parallelism strategies will thus have different benefits. With
intra-op parallelism where a single computation stage is done across multiple
GPUs, TTFT is lower when loads are low but once there are more requests, it
suffers from queuing delay. For intra-op parallelism where model layers are
sharded across GPUs, each request is slower but there is higher throughput.

Decode phase: The decode phase processes one output token at a time and is thus
memory bound because the intermediary results must be stored in the KV cache
which resides in GPU memory. Unless requests are batched heavily, GPU
utilization is low. Larger decoding batch sizes means better utilization but
its hard to increase batch size with traditional serving because prefill
benefits from small batch sizes. By separating decoding to a separate GPU and
having large batch sizes, one decoding instance can handle results from
multiple prefill instances. Decoding benefits mostly from inter-op parallelism.

While disaggregation addresses these problems, there are also some concerns
that could prevent it
- Variable prefill lengths: Prompt lengths are non-uniform and can break
inter-op pipeline balance. The solution is to use a scheduling policy that
balances execution time in each pipeline stage
- Communication overhead: Transfer of KV Cache from the prefill stage to the
decoding stage means a lot of data to be transfered. Use intra-node NVLINK for
communication and placement algorithm ensures co-location of corresponding
stages on the same node
- Design tradeoffs: Disaggregation leads to more work such as batch sizing,
parallelism type and degree, node placement, GPU resource allocation

## Method
DistServe needs to automatically find the best way to allocate resources and
choose parallelism strategies. The components needed to accomplish this are
placement algorithms and runtime scheduling.

Placement for High Node-Affinity Clusters: For clusters with high speed
interconnections like InfiniBand, perfill and decoding stages can be optimized
separately. A simulator is used to estimate the goodput of enumerating of
feasible parallelism configs. Once the best config is chosen, replicate the
instances to match the traffic rate. This simulator uses analytical models to
estimate latency. It is based on trace driven workload distributions and is
able to simulate much faster than real world profilling and is verified to be
within 2% of real measurements.

Placement for Low Node-Affinity Clusters: For clusters with limited cross node
bandwidth, transfer of KV cache between the phases becomes costly. Intra node
is used placement is used where there is NVLink. The model is broken into
segments where prefill and decoding segments are on the same node. A simulator
is used to choose the best configuration for all possible intra node
parallelism combinations.

Online scheduling: To schedule incoming requests, multiple optimization
strategies were chosen
- Load balancing: Prefill requests were routed to prefill instances with the
shortest queue. Decoding tasks were assigned to the least loaded decoding
instance
- Reducing pipeline bubbles: Long prompts can cause underutilization in
pipelines. For prefill stage, the saturation threshold or the prompt length
that utilizes the GPU fully is profiled and multiple prompts are batched to
meet this threshold. For decoding stage, the batch size was capped by the
memory usage
- KV Cache Transfer: Decoding instances pull KV caches from prefills to avoid
bursts. This meant the prefill instances needed to temporarily buffer its KV
cache
- Replanning: Workload patterns may change so systems monitor for drift and re
executes placement algorithms. This repllaning is pretty fast and can be done
hourly
- Preemption and Fault Tolerance: For larger prompts, want to preempt to allow
small batches to go through. Faults in the decoding stage can cause fault
cascades but this paper doesn't have fault tolerance mechanism

## Implementation
DistServe was implemented as different components. 
- Placement algorithm
- Restful frontend server
- Orchestration layer

## Evaluation
The authors aimed to measure per-GPU goodput, SLO adherence, scalalbility,
communication overhead, simulator accuracy, and algorithm efficiency. 4 nodes
with 32 GPUs were used with an inter-node bandwidth of 25Gbps. LLM models used
were OPT-13B, OPT-66B, OPT-175B with float 16 precision. OPT has multi-headed
attention which has larger KV cache stressing the communication overhead. This
was compared with vLLM and DeepSpeed. DistServe was able to achieve 2-4.6 times
higher request rate than vLLM and 1.6 to 7.4 times higher than DeepSpeed with
tighter SLOs

To understand the communication overhead, the different stages of model
execution was measured. Turns out, KV cache transfer was less than 0.1% of
total latency with most transfers completing in 95% thanks to NVLINK.

## Discussion
The limitations of DistServe is throughput optimized scenarios such as offline
or batch jobs that don't care about latency. For resource constrained
environments with fewer GPUs, harder to implement.
