# I/O Multiplexing

Typical socket operations such as `recv` are blocking. In order to get around
the blocking nature of these APIs, you can 1) start a separate stream of
execution to do the blocking operation 2) use asynchronous kernel APIs 3) do
I/O multiplexing.

This file talks about I/O multiplexing with `select`, `poll`, and `epoll`.
`select` and `poll` are long standing UNIX APIs but have known performance
degradation with large number of sockets. `epoll` is a Linux specific API that
scales better with more file descriptors but is not portable like the first
two.

## `select`
```C
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);
```

The `select` system call blocks until a file descriptor in one of the sets is
ready
- `readfds`: Set of file descriptors to see if reading is possible
- `writefds`: Set of file descriptors to see if writing is possible
- `exceptfds`: Set of file descriptors to see if any exception condition has
occured

Operations on an fd set is to be done strictly through the following macros.
```C
void FD_ZERO(fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_CLR(int fd, fd_set *fdset);
int FD_ISSET(int fd, fd_set *fdset);
```

The call to select modifies the sets that we pass in. This means that we also
have to keep the set of file descriptors in our own data structure as to not
lose them when `select` modifies it.

The return value of select indicates:
- < 0: An error
- 0: Call timed out
- > 0: Number of ready descriptors total out of the three sets

The main disadvantages of `select` is that you are limited to 1024 fds and you
have to make a new `fd_set` every time you want to call `select`. Then, after
`select` returns, you have to iterate over all of the fds that you are polling
and check with `FD_ISSET`.

## `poll`
```C
int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```
Poll is a similar syscall to `select` with the major differnece between how to
specify the file descriptors and events to poll for. `pollfd` looks like the
following
```C
struct pollfd {
    int fd; /* File descriptor */
    short events; /* Requested events bit mask */
    short revents; /* Returned events bit mask */
};
```
The caller initializes `events` with a bitmask that indicates which events they
are interested in for a specific fd. `poll` sets the bitmask in `revents` to
indicate which events are ready on the fd. You can used the following bitmasks
to specify the events
- `POLLIN`, `POLLRDNORM`: Data other than high priority can be read
- `POLLPRI`: High priority data can be read
- `POLLRDHUP`: Shutdown on peer socket
- `POLLOUT`, `POLLWRNORM`: Normal data can be written
- `POLLWRBAND`: Priority data can be written

The return events can have the above and the below specified
- `POLLERR`: Error
- `POLLHUP`: Hangup
- `POLLINVAL`: File descriptor is not open

The return value of poll indicates:
- < 0: An error
- 0: A timeout
- > 0: The number of pollfd in the set that are ready

## `poll` vs `select`
The kernel implementation of these syscalls uses the same underlying polling
infrastructure.

`poll` doesn't have an intrinsic limit on the number of fds you can monitor.

The performance of `poll` vs `select` start to differ once the monitoring file
descriptors becomes sparese meaning the max file descriptor you are monitoring
is high but there are only a couple. In this sparse case, `poll` can perform
better than `select`. Select monitors all file descriptors in the range 0 to
`N-1` while poll only needs to monitor the ones in the `pollfd` array.

`poll` and `select` have similar performance degradations when monitoring a
large number of file descriptors. The kernel must check all of the file
descriptors requested by these syscalls. `select` requires reinitialization of
the fd set before each call. For `poll`, the `fdset` array must be copied back
and forth between user and kernel space on every syscall. After these syscalls,
userspace has to loop over the sets again to determine which file descriptors
are ready.

## `epoll`
The `epoll` syscall scales much better with a large number of file descriptors.
Furthermore, it allows you to do level-triggered and edge-triggered
notification. Level triggered means that there is a notification whenever a
descriptor is ready, ie there is data to be read. This means you don't
necessary have to read all of the data in between notifications. Edge trigerred
notifies when there is a change in descriptor state, ie when data arrives. You
have to read all of the data at this time because leaving data unread doesn't
trigger a notification. Concisely, level-triggered reports readiness until a
condition remains true, edge-triggered reports when the condition changes.

The central data structure is an epoll instance, described by a file
descriptor. This descriptor is used to 1) record the actual descriptors the
program is interested in and 2) maintain a list of descriptors that are ready.

### `epoll_create`
```C
int epoll_create(int size);
```
Creates a new epoll instance. The size argument is a hint to the kernel on how
many file descriptors we expect to monitor (which I think is actually ignored
nowadays). It returns a file descriptor to refer to the epoll instance.

### `epoll_ctl`
```C
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev);
```
Modify the interest list of an epoll instance. Interestingly, `epfd` can't be a
file descriptor to an actual file or directory.

The `op` value can be
- `EPOLL_CTL_ADD`: Add a file descriptor to the instance. Set of events in
`*ev`
- `EPOLL_CTL_MOD`: Modify an existing file descriptor
- `EPOLL_CTL_DEL`: Remove an existing file descriptor

`epoll_event` looks like the following:
- `events`: Set of events we are interested in for `fd`
    - `EPOLLIN`, `EPOLLOUT`, `EPOLLPRI`, `EPOLLRDHUP`, `EPOLLERR`, `EPOLLHUP`
- `data`: A union, one of which can be used to convey information in
`epoll_waitt`. The kernel keeps this unchanged and supplies it on `epoll_wait`

```C
struct epoll_event {
 uint32_t events; /* epoll events (bit mask) */
 epoll_data_t data; /* User data */
};

typedef union epoll_data {
 void *ptr; /* Pointer to user-defined data */
 int fd; /* File descriptor */
 uint32_t u32; /* 32-bit integer */
 uint64_t u64; /* 64-bit integer */
} epoll_data_t;

struct epoll_event ev = {0};
ev.events = EPOLLIN;
ev.data.fd = socket_fd;

epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &ev);
```

### `epoll_wait`
```C
int epoll_wait(int epfd, struct epoll_event *evlist, int maxevents, int timeout);
```
Returns information about ready file descriptors. `evlist` contains information
about ready file descriptors in a list of `epoll_event`s. This is allocated by
the caller and `maxevents` is the size of this array. The `data` field in the
`epoll_event` is how we know what `fd` is ready so this should be set by the
caller of `epoll_ctl`.

The `timeout` value can be set as the following values:
- -1: Block until an event happens
- 0: Do a nonblocking change
- > 0: Block for up to `timeout` milliseconds

Returns
- -1: Error
- >= 0: Number of file descriptors that are ready

You can change the interest list while in a `epoll_wait` if you concurrently
call `epoll_ctl`.

### `epoll` Semantics
When you create an epoll instance, a new i-node and file description is
created. The epoll instance is associated with the file description and not the
epollfd file desriptor that is returned from `epoll_create`. (A file descriptor
is the user space integer while the file description is the kernel object that
is the underlying reference of the file descriptor). This means that if you
duplicate the file descriptor with `dup` or `fork`, you are still referencing
the same epoll instance.

### `epoll` performance
We previously said that performance degrades drastically as number of file
descriptors increase for `select` and `poll`
```text
Descriptors (N) | poll() CPU (s) | select() CPU (s) | epoll CPU (s)
----------------|----------------|------------------|--------------
             10 |           0.61 |             0.73 |          0.41
            100 |            2.9 |              3.0 |          0.42
           1000 |             35 |               35 |          0.53
          10000 |            990 |              930 |          0.66
```

This is for the following reasons
- The work to mark a file descriptor is ready is done at the time of an event.
`epoll_ctl` registers the fact that the file description we are monitoring was
added to an epoll list. When I/O operation happens, the file descriptor of that
file description that changed is moved to the ready list of the epoll instance.
Thus `epoll_wait` just has to read off of this list
- The data structure to convey which fds to monitor is built in kernel space
and there is no copying between user and kernel space (except for `evlist`)
