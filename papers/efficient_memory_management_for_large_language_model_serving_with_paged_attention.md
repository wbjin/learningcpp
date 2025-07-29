# Efficient Memory Management for Large Language Model Serving with PagedAttention

## Summary

Improve LLM serving throughput by efficiently managing Key-Value cache with
techniques inspired by virtual memory and paging techniques from operating
systems. This paper also introduces vLLM.

## Intro

LLMs generates output tokens by iterating over input and previously generated
output tokens. It is a sequential generation that is memory-bound that makes it
hard to fully utilize the computational power of GPUs.

Memory usage on GPUs when serving LLMs is mostly model parameters, memory used
for activation, and KV cache. Model parameters is constant and memory needed
for activation is small so edge is in efficiently managing KV Cache. 

Current way of managing KV cache, which is to store it in contiguous memory is
not efficient because deep learning workloads cause KV cache to dynamically
grow and shrink over time and you can't know the lifetimes of these requests
beforehadnd. Existing systems suffer from internal and external fragmentation
because they preallocate a big chunk of contiguous memory with the request's
max length. Internal fragmentation because actual request can be much shorter
than this. Other shorter requests that don't need that much memory can't take
advantage of this unused memory. External fragmentation can also happen because
max request sizes can be dynamica and hence the allocated memory can vary in
size (same problem as memory management with segmenting). Can't share memory
too for more advanced stuff (parallel sampling, beam search ??).

Use OS techniques of paging and virtual memory. Block == Page, tokens == bytes,
requests == processes.

## Background
Generating an output token depends on all previous tokens in that sequence. The
key and value vectors (?) of existing tokens are cached in KV cache. KV cache
of a token depends on what order in the sequence it appears so you can't just
do a straight token to value pair. 

Request is broken down to two phases 1) Prompt phases that goes through the
prompt. All prompt tokens are known so matmult with GPU to make it fast. 2)
Autoregressive generation phase that generates the output tokens sequentially.
Uses the key and value vectors of previous tokens that are cached and computes
the key and value vector for the new token. This cannot be parallelized so
can't fully take advantage of GPU.

GPU utilization can be improved with batching. Batching can be difficult
because 1) different request arrival times, 2) different request input and
output lengths. Fine grained batching improves this by making batching at the
iteration level instead of request level. Requests at different stages in
generation can be computed at the same time. Finished requests are taken out of
the batch and new ones are added.

## Method

KV cache is split into blocks and each block contains key and value vectors for
a fixed number of tokens. The key and values of a token can be split across
multiple blocks that may not be contiguous in memory. I don't understand the
algorithm for computing attention so will skip.

A request's KV cache is represented as logical KV blocks (like virtual pages)
that are stored as non contiguous KV blocks (physical pages). A "block engine"
(present on GPU workers), allocates GPU DRAM and splits it into physical KV
blocks. The KV "block manager" maints a "block tables" which contains the
mapping between logical and physical KV block for a request. Each entry in the
block table records the corresponding physical blocks of a logical block. It is
analgous to paging in OS where "block manager" == pager, "block table" == page
table, "block table entry" == page table entry. Logical block size == physical
KV block size so that when you fill your slots in logical KV block, new KV
block logical and physical block is used.

This also allows sharing of KV blocks. Many models generate multiple sequences
per input prompt (parallel sampling). When doing this, the KV cache of the
prompt can be shared (like how pages are shared initially on a fork). There is
actually a copy-on-write mechanism where if one sequence writes to the shared
KV block, there is a copy on that write. Each physical KV block contains a
reference count to denote the number of logical KV blocks that reference the
physical KV block.

KV blocks need to be swapped out sometimes when requests overwhelm the system's
capabilities. This is similar to swapping out pages when memory usage
approaches the limits of physical memory. vLLM does an all or nothing evition
meaning it either evits all the blocks of a sequence or none of them. This is
because all of the blocks have to be accessed to generate the next token so
there is no need to keep some of the blocks around. Additionally, when parallel
sampling is often done in sequence groups where the seqeunces are executed
together as a group. vLLM evicts swapped blocks to CPU memory. When memory
usage gets to a point where it needs to be swapped to CPU, vLLM does not accept
any new requests until all preempted sequences are completed. Once there are
free blocks on GPU KV Cache, the evicted blocks are brought back in and
preempted sequences are continued. Block swapped to CPU RAM is never greater
than the total physical blocks in GPU so swap space == GPU RAM. You can also
just recompute the KV cache. This can be pretty fast as the previously
generated output tokens can be treated as prompt tokens.
