# Memory API

## C++ member functions
In C++, non-static member functions are simply functions where the object is
passed in implicitly. For example
```C++
struct A{
    void foo() {};
}
```
is really just
```C++
void foo (A *this);
```

## `malloc` and `free`

https://sourceware.org/glibc/wiki/MallocInternals

### Chunks
`glibc`'s malloc is chunk based. A chunk is the combination of metadata with
the usable memory (the memory given to applications). `malloc` returns the
address to the usable memory, ie the payload, but keeps the metadata part
hidden.

When a chunk is allocated and given the application, the only important
metadata is the size.
```
[size | flags][payload (given to application)...]
```

When a chunk is freed, the payload is overwritten with pointers that link into
the free list and extra info about the chunk for faster reuse. Additionally, a
copy of the size is stored at the end of the chunk.
```
[size | flags][pointers, further metadata...][size]
```

The flags that are associated with a chunk are
- A (0x04): Arena flag. 0 means the chunk belongs to the main arena (brk heap).
1 means it belongs to a non-main arena (`mmap`ed heap). glibc maintains multiple
arenas (for multiple threads) and different arenas may have different memory
regions.
- M (0x02): `mmap`'d chunk. Means chunk was allocated via mmap. These chunks are
standalone and don't have neighbors and aren't in bins. They must be freed with
`mmunmap`.
- P (0x01): Previous chunk in use. 1 means previous chunk is in use, 0 means
previous chunk is free. When freeing a chunk, allocator may merge chunks with
neighbors. This makes it quick to check

`mmap` and `brk` heaps are different ways glibc gets memory from the OS.
`malloc` has the concept of arenas to which chunks belong to. The "main" arena
is managed using `brk` and is considered the process's main heap. Other arenas
are created to reduce thread contention (usually one per thread but there is a
limit) and these are allocated using `mmap`
- `brk`: Extends the break point of the process, ie extends the size of the
heap. Used for small to medium allocations and the memory is managed internally
by malloc
- `mmap`: Requests a separate memory region from the OS for larger allocations.
Each allocation is its own independent chunk that is not connected to any other
chunks.

`brk` is fast and efficient while `mmap` avoids fragmentation and allows for
huge heap growth

### Arenas and Heaps
glibc's `malloc` allows for multiple regions of the heap to be active at the
same time (allow sparse allocations I guess) for multi threading. Different
threads can access different regions without interfering. These regions are
called "arenas". A process initially starts with one main arena. Each arena has
a `prev` pointer that points to a different arena. These additional arenas are
created and expanded using `mmap` (instead of `brk` for the main arena) and
each arena has an associated mutex. By default, `malloc` limits the number of
arenas to eight times the number of CPU cores.

An arena is responsble for managing one or more "heaps". When a non-main arena
is allocated using `mmap`, the metadata for the arena will also be stored in
the allocation. It places metadata for the heap `heap_info` but also metadata
for the arena that contains:
- mutex
- bin/free lists
- pointer to top chunk

When this allocation runs out of space, the arena will call `mmap` again for
another allocation. This allocation may not be contiguous with the existing
allocation and thus has a pointer pointing back to the arena metadata in the
original allocation. The arena metadata also contains a pointer to the "top"
chunk, the chunk that refers to the biggest available chunk and thus the most
recently allocated "heap".

The arena mantains lists of free chunks based on size and history so that
suitable chunks can be found quickly. These lists are also called "bins". Note
that `malloc` doesn't keep track of chunks that were given to the user and not
freed yet. The different bins are
- Fast: Small chunks are stored in the `fastbin` list. This is a list of same
sized chunks organized as a singly linked list. Chunks in the `fastbin` are not
coalesced and are accessed in order from the front of the list. Chunks can also
be moved to other bins if needed.
- Unsorted: The first bin a freed chunk gets put into. This list remains
unsorted in hopes that another allocation can use this recently freed chunk.
Kind of like a temp bin 
- Small: Bins of equally sized chunks organized as a doubly linked list. Chunks
are coalesced before insertion. Allocation remains simple by taking the first
chunk of the correct size.
- Large: Each bin has a range of chunk sizes. You need to search for the "best
fit" and a chunk may have to be split before being given to the application.
Better for memory utilization but slower. Each chunk contains a pointer to the
first chunk of its size. Chunks are sorted in large to small.

### Thread local cache `tcache`
The glibc malloc is only aware that there are multiple threads and that is
about it. It doesn't have support for NUMA architectures or thread locality.
Each thread has a thread local variable indicating the arena that it has used
last. If this is set, a thread will block for the arena to become available. If
it is not set, it may to use an unused arena, create a new arena, or pick
another arena from the arena list.

`malloc` also maintains a per thred cache of recently freed chunks that can be
accessed without the arena locks. The `tcache` contains an array of linked
lists where each index corresponds to a chunk size and contains a pointer to a
list of chunks. Another array maintains the number of chunks per size group in
the `tcache` to prevent unbounded cache growth.

The cached chunks store pointers to the payload (memory givent to the user) and
the pointer to the next chunk is stored in the payload as well to avoid
touching allocator metadata.

### `malloc` algorithm
1. If there is a suitable chunk (exact match in terms of size), use that
2. If it is a big request, `mmap` is used. This threshold is dynamic or can be
overwritten with `M_MMAP_THRESHOLD` and there is a limit to how many of
these allocations can exist.
3. If fastbin has an appropriate chunk, use that. If there are multiple
appropriate chunks, fill the `tcache` with extra
4. If smallbin has an appropriate chunk, use that. Fill `tcache` if possible
5. If the request is large, take everything in fastbin and move to unsorted
bin, coalescing
6. Start taking chunks off the unsorted list and move to small/large bins
coalescing as you go. If a suitable chunk is found, use that
7. If request is large, look through the large bin until a suitable large chunk
is found

### `free` algorithm
1. If room in `tcache`, store there
2. If chunk is small enough, place in appropriate fastbin
3. If chunk was `mmap`d, `munmap` it
4. Coalesce if this chunk is adjacent to another free chunk
5. Place chunk in unsorted list unless it is now the "top" chunk

Note that free doesn't give memory back to the OS, it only makes it available
for reuse by the application. Memory is returned to the OS if the freed chunk
was allocated using `mmap` or of the the top chunk becomes to big after
coalescing

### `realloc` algorithm
For `mmap`d chunks `realloc` tries to call `mremap` which may or may not mean
moving of the address of the allocation.

For normal arena chunks
- If it is a reduction in allocation, may attempt to split chunks depending on
whether or not it is "worth it"
- If the allocation is growing and adjacent chunk is free and large enough,
chunks are coalesced
- If there is no way to grow the existing chunk, user needs to allocate new
memory, copy and then free
