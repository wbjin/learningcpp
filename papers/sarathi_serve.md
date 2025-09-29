# Taming Throughput-Latency Tradeoff in LLM Inference with Sarathi-Serve

## Summary
LLM inference has a prefill and a decoding stage that have different
bottlenecks where prefill is compute intensive and decoding is memory bound.
The systems at the time use prefil proritizing strategies for high througput
but generation stalls or decode prioritizing which favor latency but hurt
throughput. Sarathi-Serve splits prefills into smaller chunks which allows it
to be interleaved with decoding without stalling. Batching is also done in a
way that creates uniform compute avooiding pipeline bubbles. 

## Introduction
The tradeoffs made in inference system is between throughput and latency.
Prioritizing decoding allows for lower latency but at the cost of throughput.
Prioritizing prefill allows higher throughput but at the cost of latency spikes
due to generation spikes. The authors propose Sarathi-Serve which avoids
throughput latency tradeoff with chunked prefills where long prompt prefills
are broken into smaller chunks allowing it to be interleaved with decode
tokens. The authors also employ stall free scheduling where new prefill chunks
are added to a running batch without pausing ongoing decodes. The paper
presents that this allows 3.7 times higher serving capacity and 5.6 times
higher throughput.

## Background
This section goes over the architecture of a transformer and the inference
process of LLMs. Decoder only transformers have two key components, the self
attention module and feedforward network. Self-attention uses a query Q, a key
K, and a value V and computes the dot product attention followed by a softmax
operation. In larger models, there is often multi-headed attention and group
query attention. Feedforward network is a bunch of matrix multiplications. LLM
inference has two key phases. The prefill phase where all of the input tokens
are processed parallely in one iteration where there is good GPU utilization.
The decode phase where output tokens are processed one at a time using past
iterations KV-cache which leads to low GPU utilization without batching.
Inference systems must serve multiple concurrent requests. Bacthing different
requests together increases GPU utilization. There are different ways to do
this efficiently such as PagedAttention with vLLM or multi query attention and
group query attention. Inference can also be split into different parallelism
strategies. Tensor parallelism where each layer is split across GPUs. Pipeline
parallelism where layers are assigned to different GPUs and micro batches are
used to pipeline batches. LLM inference systems need good time to first token
and time between tokens as well as high capacity (essentially throughput).
Previous systems sacrificed one for the other where prioritizing decoding led
to lower TBT latency but caused stalls in prefill of new requests which hurt
time to first token. Prioritizing prefil allowed low time to first token at the
cost of time between tokens.

## Motivation
This section covers the current LLM serving landscape and the motivation for
Sarathi-Serve. The authors analyze that decoding benefits heavily from batching
while prefill not so much. This opens an opportunity to combine them.
Schedulers at the time prioritized one or the other when this isn't necessarily
needed. The authors also observe that prefill and decoding iterations have much
different execution time with prefills taking much longer. This causes pipeline
bubbles where GPUs sit idle waiting for a straggler to catch up. So the key
challenges that the authors observed were
- Prefill vs Decode: Prefill is compute intensive while decoding is memory
intensive. This means that these could be mixed to balance GPU usage
- Throughput vs Latency: Existing schedulers need to trade off throughput or
latency. A better system needs a stall free strategy that doesn't have to
prioritize one or the other
- Pipeline bubbles: Non uniform batch compute requirements cause pipeline
bubbles. Pipeline batches need to be constructed in a way that requires
uniform batches

## Sarathi-Serve
Chunked Prefills: To address the problem of generation stalls where prefills
can't happen until existing decode stages finish (generation stalls), the
authors decide to break long prompt prefills into smaller chunks. These smaller
chunks are processed across multiple iterations instead of just one iteration
for normal prefill. These smaller prefill chunks are added to existing batches
and is essentially "piggybacking" on memory intensive decode stages. This
reduces time to first token and eliminates generation stalls while keeping a
low time between token latency since decoding doesn't have to be stopped.

Stall free batching: In the systems at the time, introducing new prefill
required ongoing batches to pause which creates generation stalls. The solution
to this was to schedule hybrid batches of ongoing decodes and prefil chunks
from ongoing or new requests. This is done by starting with packing all decodes
and then adding on any ongoing prefill chunks. If there is room, new prefill
chunks are added.

Uniform compute batches: By mixing the prefill and decode batches in a
controlled ratio, each batch requires roughly the same amount of compute which
makes it ideal for pipeline parallelism because it reduces pipeline bubbles.

The key challenge in this implementation is to balance the token budget. Having
smaller chunks means lower decode latency while having larger chunks means
higher GPU efficiency. Adding chunks adds memory overhead as more frequent
KV-cache access is needed so choosing an ideal token budget is important. The
authors do this by doing a one-time profiling to determine optimal chunk/token
size based on performance objectives, hardware capabilities, and parallelism
config.

The authors implemented Sarathi-Serve on top of vLLM. It leverages large batch
sizes without stalling allowing high throughput. It has low TBT latency by
prevneting generation stalls. It has high GPU utilization for efficient hybrid
batches and optimizing for memory and compute. It has high scalability as it
works across TP and PP.

## Evaluation
The authors tested the system with various sized modles ranging from 7B to 180B
on various number of GPUs from one to 8. The metrics that they collected were
99th percentile TBT latency and 50th percentile TTFT latency.

Capacity: Across different models, Sarathi-Serve saw a 2.6 to 6.3 times gain
over vLLM and 2.8 to 5.5 times over Orca. 

Latency throughput tradeoff: The auhtors tested Sarathi-Serve and vLLM with
different token budgets to test for different latency targets. Sarthi-Serve
consitently had higher capacity at all latency targets

Pipeline bubble analysis: vLLM and Orca have pipeline bubbles with PP.
Sarathi-Serve showed that with TP and PP, it had higher capacity than vLLM and
lower median TBT due to the uniform compute batches

Ablation studies: The authors also studied the cost of small chunk prefills and
full prefills and showed that even at small chunk sizes, the overhead was
moderate. Chunked prefills allow lower TBT but hurt TTFT. Hybrid batching
allows lower TTFT but causes generation stalls. Combining both yields the best
results
