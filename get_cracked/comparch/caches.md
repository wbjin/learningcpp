# Caches

TLDR, CPU clock speeds have gotten much fatser than memory clock speeds. As the
two diverge, memory fetches become more and more expensive relative to a CPU
cycle. The modern solution is to put tiered caches that are closer to the CPU
core.

```txt
Assumes a 4 GHz CPU: 1 cycle = 0.25 ns.
Values are representative estimates, not measurements of one machine.

Location          Access time       CPU cycles at 4 GHz    Typical ratio
----------------  ----------------  ---------------------  -------------
Register op*      ~0.25 ns           ~1                     1x
L1 cache          ~1–1.25 ns         ~4–5                   5x
L2 cache          ~3–5 ns            ~12–20                 15x
L3 cache          ~10–25 ns          ~40–100                50x
RAM (DDR5)        ~60–120 ns         ~240–480               400x
NVMe SSD          ~20–100 us         ~80,000–400,000         200,000x
SATA SSD          ~80–200 us         ~320,000–800,000        400,000x
Mechanical HDD    ~5–15 ms           ~20–60 million          40,000,000x

Representative latency ratio:
Register : L1 : L2 : L3 : RAM : NVMe SSD : HDD
       1 :  5 : 15 : 50 : 400 :  200,000 : 40,000,000
```

## L1 Cache
L1 caches sit on the same piece of silicon as a CPU core and is thus the
closest non-register memory. L1 caches are typically made of static RAM cells
(SRAM) which typically have four to six transistors per bit (compare to one for
DRAM). This makes SRAM significantly more expensive but also much faster to
access.

SRAM cells are faster to read and write than DRAM cells because SRAM cells
maintain the bit in a stable circuit while DRAM cells stores each bit as an
electrical charge that needs to be restored after a read. An analogy would be
that SRAM is a light switch that is either on or off and checking its position
is quick and doesn't change it. DRAM would be a bucket's water level where you
need to pour some water out to read the charge. After the reading, you must
pour the water back in to restore the level. It also leaks, meaning you need to
periodically restore the charge. While SRAM may be faster, it requires more
silicon space and is much more expensive. You can pack DRAM cells more densely
at cheaper cost.

The L1 cache is split into instruction and data caches (iCache vs dCache). The
sizes of these can range from 32~64KB and 32~46KB respectively for a total of
64~112KB per core.

## L2 Cache
The L2 cache is also typically made out of SRAM. Depending on the CPU
architecture, it may be per core or shared amongst a group cores. Some even
have private and shared L2 caches. Its sizes can range, but a rough estimate is
1-4MB per core, where it can either be private or shared.

## L3 Cache
The L3 cache is made out of SRAM and is typically outside of individual cores
and shared by a group of cores, whether that be cores on a die or a particular
group of cores. They can be roughly 16-64MB in size.

## L0 cache
L0 caches are even fatser than L1 caches with access speeds of single digit
cycles. It only holds a few KB of specialized data such as decoded micro ops.

## Cache Misses
Cache misses can be categorized into one of three Cs:
- Compulsory: First time seeing the data, all data must be brought into cache
at some point with a cache miss (unless there is some prefetching involved)
- Capacity: The working set is too lareg and previously cached data needs to be
evicted
- Conflict: Too many lines map to the same cache set, even though the rest of
the cache has space

## Cache Lines
A cache line is a fixed size block of bytes that is grouped together in cache
and in RAM, typically 64 bytes in modern CPUs, ie 0-63 is one atomic unit,
64-127 is another. Data transfer between caches and RAM happen at cache line
sizes meaning even if you access 4 bytes of a cache line, the neighboring bytes
will also be brought into cache.

## Tag RAM
Tag RAM is a small portion of memory inside caches that record which parts of
main memory each cache frame belongs to. It holds three critical pieces of
information. Each cache frame (different from the 64 byte unit of transfer)
stores a tag, the actual data, and some status bits.

When a CPU accesses memory, the address used to access can inform where in the
cache to look for the data. Some parts of the address can be used to determine
which set in the cache to look for. Then, a portion of the address is compared
against the tag of the cache line to make sure this is data that we want. Then,
another portion of the address can be used to determine the offset within the
cache line to fetch the data from.
```txt
16 bit address: [ Tag: 6 bits ][ Set index: 4 bits ][ Byte offset: 6 bits ]

Binary address:  000100  1000  110100
                ------  ----  ------
Tag:            000100          = 4
Set index:              1000    = 8
Byte offset:                  110100 = 52
```

## Cache Assosciativity
Because caches are smaller than system memory, there are various ways you can
place data frames on a cache to minimize unwanted cache misses and to minimze
cache look up times. This is called a cache placement policy.

### Fully Associative
The simplest scheme is fullly associative mapping. With this scheme, you can
put data frames anywhere in the cache. The negatives of this approach is that
you have to do a linear search of all of the cache lines in a cache (which can
be thousands to millions) to see if data is available in the cache which adds
latency to a fetch and grows linearly with cache size (although this search can
happen in parallel in hardware).
```txt
                 Cache slots
           +-----+-----+-----+-----+
           |  0  |  1  |  2  |  3  |
           +-----+-----+-----+-----+
              ^     ^     ^     ^
               \    |     |    /
                \   |     |   /
                 Any RAM block
                 can go in any
                   cache slot
                       ^
                       |
                   RAM blocks
     +-----+-----+-----+-----+-----+-----+
     |  A  |  B  |  C  |  D  |  E  | ... |
     +-----+-----+-----+-----+-----+-----+
```

### Direct Mapping
In direct mapping, each data frame in memory is associatd with a specific cache
slot and multiple data frames may need to share a single cache slot. in the
example below, data frames 0, 4, and 8 need to share the slot at 0. This
greatly reduces the time spent looking in a cache because you know that an
access to 0, 4, and 8 will only be in slot 0. However, this may drastically
increase cache evictions if your working set shares a cache slot, ie you need
to access frames 0, 4, and 8 again and again which conflicts for one slot

```txt
                 DIRECT-MAPPED CACHE

                     Cache slots
               +-----+-----+-----+-----+
               |  0  |  1  |  2  |  3  |
               +-----+-----+-----+-----+
                  ^     ^     ^     ^
                  |     |     |     |
RAM blocks     +-----+-----+-----+-----+
               |  0  |  1  |  2  |  3  |
               +-----+-----+-----+-----+
                  |     |     |     |
               +-----+-----+-----+-----+
               |  4  |  5  |  6  |  7  |
               +-----+-----+-----+-----+
                  |     |     |     |
               +-----+-----+-----+-----+
               |  8  |  9  | 10  | 11  |
               +-----+-----+-----+-----+
```

### Way Set Associative
A middle ground between direct mapping and fully associative is a set
associative cache. This allows you to limit the number of lines you need to
search while allowing for potential conflicts to the same set. A direct mapped
cache is essentially a 1-way set where each set in the cache only has one
frame.

An example of this is a 4-way set assosciative mapping where the cache is split
into sets of 4 frames. In the example below, frames 0, 2, and 4 can go anywhere
in set 0 while 1, 3, and 5 can go anywhere in set 1. This way, if there is a
conflict within the same set, there doesn't necessarily have to be an eviction.

```txt
              4-WAY SET-ASSOCIATIVE CACHE

              Set 0                 Set 1
       +----+----+----+----+  +----+----+----+----+
       |    |    |    |    |  |    |    |    |    |
       +----+----+----+----+  +----+----+----+----+
          Any of 4 slots         Any of 4 slots
                ^                     ^
                |                     |
RAM blocks   +-----+               +-----+
             |  0  |               |  1  |
             +-----+               +-----+
             |  2  |               |  3  |
             +-----+               +-----+
             |  4  |               |  5  |
             +-----+               +-----+
             | ... |               | ... |
             +-----+               +-----+
```

Modern CPUs often use set associative caches with 8-12 ways. 

## Write Policies
There are two ways to deal with stores in caches. The first is to write-through
where each load updates all of the cache lines in the hierarchy including
system memory. The other is write-back where you write to lower levels of cache
hierarchy as they are evicted.

Write through is simpler to implement as all data is supposed to be
synchronized. However, every write triggers traffic to the next level and heavy
write traffic can cause delays. Write back can reduce traffic as changes can
stay local but there needs to be additional bookkeeping to do writes on
evictions.

Modern processors use write back with write allocate where a write modifies the
cache line and marks it dirty. If a write causes a cache miss, the cache line
is first brought into cache and then written to.

## Inclusive vs Exclusive Cache
With hierarchical caches, there are different ways you can implement the
eviction of caches. Inclusive caches duplicate data across cache levels. L3
includes everything in L2 and L2 includes everything in L1. Exclusive caches
keep different data at every level.
```txt
INCLUSIVE: L2 includes everything in L1

L1:  [ A ][ B ]
L2:  [ A ][ B ][ C ][ D ]

EXCLUSIVE: L1 and L2 contain different blocks

L1:  [ A ][ B ]
L2:  [ C ][ D ][ E ][ F ]
```

In an inclusive cache, when a cache line gets evicted from L1, L2 already has a
copy of that dat which needs to be updated. Inclusive caches make tracking
which blocks exist easier but at the cost of wasted capacity. Exclusive caches
allow more total data to be in caches but evicting cache lines cause needs more
management. The relationship can also vary between different levels, ie L3 and
L2 might be inclusive while L2 and L1 might be exclusive.

Modern processors also use non-inclusive/exclusive and
mostly-inclusive/exclusive behavior, meaning there may not be strict
enforcement of relationships. For example, AMD may use inclusive caches for L2
and L1 but L3 to core caches is mostly exclusive, meaning L3 largely holds
evicted lines but duplication is allowed. Intel may do non-incllusive L1 and L2
caches meaning L2 need not contain all the contents of L1.

## Cache Coherency Protocols
The memory model abstraction allows various differing cache implementations. In
SMP systems, the caching system must maintain the following memory model:
- A dirty cache line cannot be present in any other processor's cache
- Clean copies of the same cache line can exist in arbitrarily many caches
simultaneously

The protocol for solving this problem is called the MESI protocol
- Modified: The local processor has modified the cache line. This means no
other cache has this cache line
- Exclusive: Not modified but no other cache has this cache line
- Shared: Not modified but shared with other caches
- Invalid: Cache line is invalid, ie unused

These four state can be used to implement efficient write back cache coherency.

```txt
                MESI PROTOCOL TRANSITIONS

  M = Modified    E = Exclusive    S = Shared    I = Invalid


  LOCAL READ
  ----------
  [M] ──> [M]       [E] ──> [E]       [S] ──> [S]

  [I] ──┬──> [E]   No other cache has a copy
        └──> [S]   Another cache has a copy


  LOCAL WRITE
  -----------
  [M] ──> [M]
  [E] ──> [M]
  [S] ──> [M]
  [I] ──> [M]


  REMOTE READ
  -----------
  [M] ──> [S]
  [E] ──> [S]
  [S] ──> [S]


  REMOTE WRITE
  ------------
  [M] ──> [I]
  [E] ──> [I]
  [S] ──> [I]
  [I] ──> [I]


  Local  = access by this core
  Remote = access by another core
```

If a processor wants to read a cache line that is modified on another cache,
the modifying processor has to send the contents of the cache line to this
processor as well as the memory controller. Then, the cache line is marked as
shared. If this operation was a write instead, ie a processor wants to write to
a cache line that is cached on another processor, the other process neads to
share the contents of the cache line and mark the state as invalid. This is
known as the Request for Ownership operation and is comparatiely expensive.

If a cache line marked as shared is written to, the local cache line must be
marked as modified and all other cache lines in other caches must be marked as
invalid. This is also a RFO operation. This announcement happens about writing
to a shared bus that the memory controller and private caches listen on.

The exclusive state is similar to the shared state except operations don't have
to be announced on a shared bus.

RFO operations are costly, as these operations must be distributed amongst all
the processors on a system. Thus, the time it takes for an RFO operation is the
slowest response from a processor in response to a RFO.

## Cache access mechanisms
Caches are virtually indexed, physically tagged. This is a combination of
virtual address accessed caches and physical address accessed caches. Virtually
accssed caches have the benefit of quick access times without needing address
translations by the MMU (which may incur PTW costs if there is a TLB miss).
However, this means caches have to be flushed everytime there is an address
space switch, which happens on every context switch. Physically indexed caches
get around the need of cache flushes but may need to wait for physical address
translation by the MMU for every access which defeats the purpose of the cache.

VIPT does both. Caches are accessed with virtual addresses in the following
manor in a 64 bit system.

1. The cache index is used to select the set to search for in the cache.
2. Simultaneously to cache access, the TLB is searched for a virtual to
   physical page translation, incurring PTW costs if there is a miss
3. The physical page number and the tag of the cache lines in the indexed set
   is compared to get the correct enetry.
4. If there is a match, the line offset is used to index into the cache line
   and get the data that is actually needed

```txt
64-bit VIRTUAL address
63                              12 11             6 5             0
+--------------------------------+----------------+---------------+
|      Virtual page number       |   Cache index  |  Line offset  |
|            52 bits             |     6 bits     |    6 bits     |
+--------------------------------+----------------+---------------+
                |                        |                |
                v                        v                |
         TLB translation          Select 1 of 64 sets     |
         (page-table walk                |                |
          on a TLB miss)                 |                |
                |                        |                |
                v                        v                v
       Physical page number      Read tags and data   Select byte
                |                 from selected set   within line
                v                        |
       Compare PHYSICAL tag <------------+
                |
                v
        Valid match = cache hit
```

While VIPT gets around both problems, there is still the challenge of aliases.
With virtual memory, it is possible for multiple virtual addresses (either in
the same process or different processes) to refer to the same physical page.
Aliasing complicates cache accesses because the cache line you are looking for
may not be in the cache set you are searching.

Luckily, there is a way to solve this alias problem elegantly. Virtual
addresses have two components, the virtual page number which is used to index
into page tables and the offset within a page. During virtual to physical
translation, the only part that changes is this page number. The offset within
the page remains the same. This means that even if two different virtual
addresses are aliasing to one physical address, their offsets are the same. We
can take advantage of this and limit indexing of the cache to within this
offset portion. The size of the offset is dependent on the page size. For 4KB
page sizes, the offset is 12 bits long. Furthermore, the amount of bits we can
use for the cache indexing depends on the cache line size. For 64 byte cache
lines, we need 6 bits for the offset into the cache line. This means that we
have 6 bits remaining for the number of sets we can have in a cache, ie 2^6 =
64 sets in a cache. If we have 8 ways in a cache, that means that the total
cache size is bound to 64 * 8 * 64 = 32KB.

```txt
Virtual address A:
+----------------------------+------------------+------------------+
| Virtual page number A      | Set index        | Byte offset      |
|                            | 6 bits [11:6]    | 6 bits [5:0]     |
+----------------------------+------------------+------------------+
              |              |<----- 12-bit page offset ---------->|
              |                            ||
Virtual address B:                         || Identical
+----------------------------+------------------+------------------+
| Virtual page number B      | Set index        | Byte offset      |
|                            | 6 bits [11:6]    | 6 bits [5:0]     |
+----------------------------+------------------+------------------+
              |                            |
              v                            v
     Both translate to the          Both select the
     SAME physical page             SAME cache set
              |                            |
              v                            v
     SAME physical tag       +------------------------------------+
              |              | Selected set: 8 ways                |
              |              +----+----+----+----+----+----+----+----+
              +------------->| W0 | W1 | W2 | W3 | W4 | W5 | W6 | W7 |
                 Compare     +----+----+----+----+----+----+----+----+
                 all tags                  |
                                           v
                              SAME tag match, SAME cache line
                                           |
                                           v
                              Byte offset selects SAME byte


                        CAPACITY LIMIT

  4 KiB page       = 12 page-offset bits
  64-byte line     =  6 byte-offset bits
  Set index        = 12 - 6 = 6 bits
  Number of sets   = 2^6 = 64

  64 sets x 8 ways x 64 bytes = 32 KiB maximum
```
