# System Design In an Hour

From https://www.youtube.com/watch?v=iYIjJ7utdDI

## Databases

### Indices
Indices are ways to speed up read queries in databases. Instead of having to a
sequential scan to find the data, we can do random access using things like
- Hash Indexes: Keep the location of data in disk, in a hash map in memory.
This allows constant time read and write (not really because disk arms need to
spin but effectively) but it means the keyset needs to fit in memory
- B+ Trees: Organize keys as trees in disk. Follow the tree structure to get
the key that you want. log n read and write but data from adjacent keys are
stored aly on disk making range queries easier
- LSM Trees and SSTables: Writes are first sent to an in memory BST. This is
flusehd to an immutable sorted table on disk eventually. Reads first check the
in memory BST, then check the SSTable. Writes and reads are log n but in memory
writes and reads make it faster

### Transactions
Transactions are a set of operations that are atomic. Databases use a write
ahead log where before writing to actual disk. Once the operations are all
actually written, mark them as committed. On a crash, uncommited operations are
reversed. This also means transactions can be reversed midway through.
Transactions can also be serializable where concurrent writes happen in serial
so that you don't have inconsistent state. This can be done by just running the
database on a single thread. It can also be done with pessimistic concurrency
control where a transaction holds locks for all rows that are affected to
prevent concurrent updates. There is also optimistic control where you do the
transaction without locks and after, check that there were no concurrent
acccesses and abort the transaction if there was. Most SQL databases like MySQL
and Postgre support transactions. However, NoSQL databases often don't support
ACID transactions because they are not always needed and read and writes can be
sped up without them.

### Row vs Column Storage
Row storage keeps all of the data in a row together in disk. This is useful if
you are reading/writing on a per record basis where you need data from the
entire or a big part of the row (think user facing applications like account
details being stored together). For analytical workloads where you typically
want to expect one column across multiple rows, it makes sense to keep the data
in the same column together. This allows you to do things like aggregations and
filtering quicker.

## Data Serialization
Data needs to be serialized into bytes when being stored on disk or being sent
over the network. The easiest solution is JSON where you have a field name and
the corresponding value. However, if both readers and writers agree on a
schema, you can use things like protobufs where you can omit the field names.

## Replication
Databases fail so multiple copies of dasta is needed to not lose data.
Replication also allows performance improvement because operations can be
spread across different replicas. Replications can have different consistency
guarantees. Strong consistency guarantees that on a write, all replicas must
see and apply that write before the operation is externalized to the writer.
This means all subsequent reads will also get the most up to date write.
However, this comes at the cost of performance. Eventual consistency is where
only a subset of replicas have to apply that write and the operation can be
externalized before all replicas have the most up to date write. This improves
performance but susbequent writes can potentially have stale data.

### Single Leader
All writes go to a single leader and writes are replicated either synchronously
or asynchronously to the backup replicas. This is very simple to implement but
the write throughput is limited by the leader. The leader is replaced by a
backup if it fails and the backup may or may not have the most up to date
writes depending on the synchronocity of the write replication. There is also
some down time when there is a leader transition.

### Multi Leader
Writes can be servied by multiple replicas but now it requires consensu between
these different replicas. Consensus can be very hard. There are many different
ways to have consensus
- Logical clocks that order events where concurrent writes can happen but
dependent writes must be serialized
- Siblings where you store conflicting writes and let the user merge or choose
which one to keep

### Leaderless
There are no leaders and writes and reads can go to any replica. This means you
have to write to some set of replicas before externalizing the write which can
introduce tail latency. For leaderless systems, consensus algorithms like Paxos
and Raft are used.

## Sharding
A single database may not be able to store the entire set of data or it may not
be able to service the traffic. Sharding is the splitting of data across
multiple nodes. Sharding can be done in various ways such as 
- Key range: Partition the table is ranges of keys
- Range of key hash: You can hash the key and set ranges of hashes for each node. 
- Consistent hashing: Hashing and then moduloing will mean shards have to move
when there is some change in the number of replicas. Instead, you can have each
meachine assigned to a location on a circle and that machine can be responsible
for the hashes that fall in between it and the next machine on the circle.

## Batch Processing
A batch processing framework abstracts away the difficulties of concurrent data
processing. Often times, you can specificy the operations you want to do on the
data, how you want it to be partitioned, allow checkpointing to avoid
restarting the entire job.

### Stream Processing
Stream processing is where there is a producer and consumer. In between, a
message broker routes messages from the producer to the consumer. This is often
used for asynchronous processing of application events, joining multiple data
streams together, and time based grouping of data. The message broker can store
messages in memory until there is a confirmation from a consumer. The broker
also does load balancing like round robin. Message brokers can also be log bsed
(Kafka) where messages are stored in disk. The broker keeps track of what
message the consumer last read in case of failures. Disk based message brokers
have replayability where the message can be replayed down the line. Consumers
can also be stateful where you want to maintain some state for workloads like
aggregation. Consumers can also store there state so that you don't have to
replay all of the messages again.

## Caching
- Write Through: Writes go to the cache as well as the disk. This ensures consistency between
cache and disk but is slow. 
- Write Around: Data is only ever written to disk. Cache misses brings the data into the cache.
- Write Back: Cache stores writes for some time, then eventually writes to disk

## Load Balancing
You want to distribute load across different machines. You can do simple things
like round robin or you can do consistent hashing where requests from the same
user go to the same server because the server may have some cached state. Load
balancers also need to be fault tolerant so you can have multiple load
balancers running at the same time or you can have an active load balancer and
others that don't do anything.

## System Design Interview Patterns

### Contending updates
Many writes to the same key that creates conflict. Solutions are
- Naively do everything on one database with a lock
- Multiple database leaders that eventually converge 
- Stream processing: Serialize events and process in small batches

### Derived data
Two datasets need to be kept in sync with one another. For example, global
secondary indexes, data transformations on slow database with a faster read
view. Solutions are
- Two phase committing which slows down writes but keeps them in sync
- Changing data capture where when updating one database, you write it to a log
based message broker and then use that to update the other view

### Fan Out
Deliver data at write time directly instead of waiting for read queries.
Examples such as push notifications, news feed, stock price delivery. Solutions
are
- Synchronous delivery to every interested part
- Asynchronous deliver with stream processing where message broker has the
message and consumer figures out who the interested parties are and sends it to
them

### Proximity Search
Find close items in a database
- Use geospatial index that is sharded based on geographic areas. 

### Job Schedule
Run series of task on worker in a cluster
- Can do round robin in a log based message broker but a long lasting job can
delay the other jobs
- Instead do a memory message broker that does round robin and pushes to the
consumers

### Aggregation
Want to aggregate data by some key or time
- Write to a distributed database and run a batch job
- Use a stream process framework where all messages go into a log based message
broker partition based on aggregation key
