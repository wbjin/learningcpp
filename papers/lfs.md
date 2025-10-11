# The Design and Implementation of a Log-Structured File System

## Summary
This paper introduces Log-Structured File System (LFS). It is a new file system
design that improves performance by writing modifications to disk sequentially
in a log like structure which optimizes for write dominated workloads.

## Introduction
The authors introduce a new disk management technique called LFS. The
motivation is that CPU speed has improved rapidly while disk speeds have lagged
behind. This causes applications to be more disk-bound. LFS is a file system
architecture that improves disk usage efficiency. It based on the assumption
that file reads are mostly satisfied by memory caches. Because memory size also
grows rapidly, cache sizes will also increase and most disk activity will be
write dominated. Prior systems only used logs temporarily for writes and
recovery. LFS uses the log as a permanent on disk structure. The main challenge
is to ensure large free areas are always available for writing. The solution
used in the paper is to divide disk into segments and use a segment cleaner to
compact live data from fragmented segments. A prototype is built into the
Sprite operating system and benchmarks show that it outperforms Unix file
systems by over 10x for small writes.

## Design for file systems of the 1990s
File system design is motivated by changing hardware and workload demands. This
means existing file systems have shortcomings for new workloads. 

### Technology
The three main components that affect file system design are cpu, memory, and
disk. Processor speeds increase exponentially which means more pressure on
other components. This makes the disk a bottleneck. Disk systems improve in
cost and capacity but not in performance. Main memory capacity also increases
which means larger file caches reducing read traffic to disk. This means disk
traffic will be dominated by writes so write efficiency becomes more critical
than reads. 

### Workloads
Workloads at the time, office and engineering applications, have many small
files which means random I/O and heavy metadata updates. Large sequential file
access is not as big of a priority. Sprite LFS is optimized for small file
efficiency but the same techniques used for small files can perform well for
large files.

### Problems with existing file systems
The two major problems of traditional FSes are fragmentation and synchronous
writes. Existing systems like Unix FFS spread related information across the
disk which leads to many small I/Os and many disk seeks. Synchronous writes
also force applications to block while writing metadata which limits
performance and negates the benefit of caching. 

## Log-Structured File Systems
The main idea of LFS is to buffer file system changes and write in one single
operation. This makes small random writes larger sequential ones which can
maximize disk bandwidth. 

### File Location and Reading
Sprite LFS supports efficient random-access reads and not just sequential
scans. It does this by maintaining indexing structures like inodes and indirect
blocks just like Unix FFS. Inodes in LFS are written to the same log as actual
data unlike Unix FFS which has fixed disk positions for inodes. It maintains an
inode map to track the location of inodes in the log. This inode map is divided
into blocks and also stored in the actual log. It can be referenced using a
checkpoint region (which is fixed) but most inode lookups are fast because it
is just cached in memory.

### Free Space Management: Segments
Free space management is the hardest challenge in a log file system. As the log
wraps around the disk, space becomes fragmented due to file deletions and
overwrites. The two choices for solving this is threading which is to just
leave the log fragmented and write to the free spaces. This leads to
fragmentation which negates the benefit of LFSes. THe second is copying which
introduces extra overhead. SpriteLFS uses a hybrid approach that divides the
disk into fixed size segments. Segments are rewritten as a whole and threading
occurs on a segment basis. Segment sizes are chosen so that copies take much
longer than seeks which keeps disk usage high.

### Segment Cleaning Mechanism
Segment cleaning is reclaiming space by copying live data to a different
segment. Each segment has a segment summary block that lists the contents which
helps identify the file and position of each block during cleaning and crash
recovery. In order to identify live blocks, segment entries are compared with
indoe references. LFS uses version numbers where if a block's version number is
outdated, it is discarded. LFS eliminates the need for free block lists which
simplifies the design and recovery process.

### Segment Cleaning Policies
The decisions that a LFS needs to make is when to clean, how many segments,
which segments, and how to group live data. Sprite LFS cleans when the number
of clean segments falls below a threshold and stops when a sufficient number of
clean segments have been restored. A write cost metric is used to evaluate
cleaning efficiency where it includes overhead from cleaning and is express
relative to the ideal full bandwidth write. Write const increaes with the
utilization of clean segments. A segment with no live blocks has a cost of 1
and higher utilization leads to higher cleaning cost. LFS cleans segments with
utilization under 80% which leads to better performans than optimized FFS which
requires cleaning segments with utilization below 80%. The key idea is that
what is important is the utilization of the specific segment and not disk wide
utilization. A good cleaning strategy picks least utilized segments. The goal
is to have a bimodal distribution of utilization. This makes cleaning more
effective and predictable.

### Simulation Results
A simulator was built ot test different cleaning strategies in artificial
workloads. The simulator had two access models, a uniform and hot and cold
where 10% of the files got 90% of the writes. Simulations with uniform access
and greedy cleaning achieved reasonable write costs where variations in segment
usage helped identify low utilization segments. Hot and cold with greedy
cleaning resulted in worse performance because cold segments lingered at mid
level utilization delaying cleaning efficiency. The insight from this is that
cold segments tie up space longer and should be treated differently. Cleaning
hot segments leads in wasted work because data changes frequently. Thus the
authors introduced a cost benefit policy where the free space as well as the
age of data was considered where more free space and older data is prioritized.
This policy led to a bimodal distribution in utilization which is what the
authors wanted to see. This approach out performed greedy cleaning across
access patterns and reduced write cost by 50%.

### Segment Usage Table
To support cost benefit cleaning, LFS maintains a segment usage table that
stores live byte count and latest modification time for each segment. Usage
entries are updated on deletions or writes. To allow sorting by age of live
blocks, each segment records the age of the youngest block.

## Crash Recovery
On a crash, a file system may be left in an inconsistent state so recovery is
required on boot. In Unix FFS, the entire disk had to be scanned but in LFS,
only the end of the log has to be examined. LFS uses checkpoints and roll
forward recovery.

### Checkpoints
A checkpoint represents a consistent complete state of the file system. Sprite
LFS has a two phase checkpoint where it first flushes all changes and then
writes a checkpoint region to a fixed location in disk. This checkpoint region
includes the inode map and segment usage table locations. This allows the
system to rebuild memory state during reboot using only this region. For
reliability, Sprite LFS has two checkpoint regions that is written to
alternately. On a crash during checkpointing, the most recent valid one is
used based on timestamps. Sprite LFS does checkpointing periodically and on
unmount and shutdowns. This means there is a tradeoff between recovery time and
overhead during normal operations.

### Roll Forward
After a crash, Sprite LFS could discard all post checkpoint log data but that
means the most recent writes could be lost. To avoid data losses, Sprite LFS
scans post checkpoint segments to recover valid data. Roll Forward uses the
segment summary blocks to locate and restore file data. If there is a new
inode, the inode map is updated to point to it. If data blocks are found
without a corresponding inode, they are ignored to avoid corrupting the disk.
Roll forward also updates the segment usage table. LFS systems have to maintain
consistency between directory entries and inode reference counts for proper
file deletion. Sprite LFS logs all directory operations in a directory
operation log which is used to reconcile inode and directory state. After
recovery, a new checkpoint is written to reflect the changes made during
recovery. 

## Experience with Sprite LFS
Sprite LFS had around 30 daily users. The authors were concerend that a LFS
would introduce more implementation complexity but it turned out that it didn't
(around the same as Unix FFS). In day to day usage, Sprite LFS seems to feel
similar to Unix FFS.

### Micro benchmarks
Micro benchmarks were used to evaluate best case performance of Sprite LFS and
SunOS that had a UNix FFS. Results showed that LFS is around 10 times fatser
for creating and deleting small files. Reads were also faster because of the
compact sequential layout. It also performed well on large file workloads where
LFS had higher write bandwidth and similar read bandwidths with SunOS. Random
writes are faster in LFS because they are converted to sequential writes. For
sequential reads of files written randomly, SunOS performed better due to
fragmentation in LFS. LFS and FFS have different disk locality. FFS assumes
logical locality while LFS is for temporal locality.

### Cleaning Overheads
Cleaning statistics were gathered over several months of production usage.
Workloads ranged from home directories to kernel sources and virtual memory
swaps. Even at 60-75% utilization, over half of the segments cleaned were
empty. Real systems ended up outperforming simulations because real files vary
in size and are often written or deleted in bulk and some files are extremely
cold allowing LFS to avoid cleaning their segments altogether. Cleaning could
be further improved to better tune on how many segments are cleaned at once.

### Crash Recovery
Roll Forward was timed in test environments where recovery time depended on
file size, count, and data written since last checkpoint. It can range from 1
to 132 seconds and a one hour checkpoint interval resulted in average recovery
time of around 1 second. In the worst case where 150 MB of writes per hour led
to 1 second of recovery time per 70 seconds of checkpoint interval.

### Other overheads
- 99% of live on disk dast is file content and indirect blocks
- 13% of log traffic is metadata (inode, inode map)
- High metadata overhead due to short checkpointing intervals
