# Desgining High Throuhgput Stock Exchange

## Problem Requirements
1. Match buyers and sellers of stock
2. Provide ordered list of all operations that everybody can agree
3. Provide ordered list of all activities pertaining to each client (placed
   orders, cancelled orders, filled orders) back to each client

Exchange multicasts market data using UDP because TCP has one to one connection
and you can't guarantee that all messages are sent out at the same time. UDP
multicast allows this to happen at the switch level using broadcasting.
However, UDP messages can be dropped and received out of order. We can deal
with this using retransmitters where there are nodes sitting on the LAN just
listening to the UDP multicast messages. If a client misses a sequence, it can
reach out to the retransmitter to get that sequence.

Matching engines need fault tolerance but with synced state. Matching engines
also run in memory so if it goes down, data can be lost. A naive approach is to
have a primary backup system. Primary and backup need to get the orders in the
same order however. This means we need some sequencer that decides the absolute
order of events. Sequencer receives client orders, broadcasts to both primary
and backup. This way, there is an agreed ordering of events. Another thing we
can do is have the primary broadcast its output to the backup or through UDP
multicast. This way, the backup doesn't have to see the input, rather it just
has to keep track of the output of the primary and apply it in order. Even if
we lose some messages because the primary goes down, the backup can generate
the missing messages. This is because the backup is running the same code as
the primary. If it has all the state before the primary processesd the event
and the first sequence of events after the primary processed the event, it can
figure out what events may follow.

The matching engine can also be partitioned by symbol. All of the symbols have
to be one the same matching engine but different symbols may be on different
matching engines. However, some trading strategies require atomicity across
symbols so potentially across partitions. This requires a distributed
transaction like a two phase commit which may not even be possible because of
the performance implications.

The matching engine needs to ensure ordering of orders/cancels/fills etc but
also the ordering based on a client. You might have a lock per client
identifier to ensure concurrent events are executed sequentially.
