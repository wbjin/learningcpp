# Destroying Processes
Processes can die after finishing execution of all of its code and calling the
`exit` function or when a process group is terminated which can happen when a
sginal that the process cannot handle is received or when there is a CPU
exception while running in kernel mode.

## Process Termination
There are two system calls for terminating user processes
- `exit_group()`: For terminating a full thread group. Invoked by the `exit()`
C library function
- `_exit()`: Terminates a single process regardless of any other processes in
thread group. This is called by the `pthread_exit()` in the LinuxThreads
library

### do_group_exit() function
Kills all processes belonging to the thread group of the current process. One
of the parameters is process termination code which is either a value specified
in `exit_group()` system call for normal termination or an error code supplied
by kernel for abnormal termination. It terminates all the processes in the
thread group by sending a `SIGKILL`

### do_exit() function
Handles process termination. Removes references from kernel data structures.
Also invokes the scheduler to make sure that a new process is running.

## Process Removal
Unix operating systems allow querying the kernel to obtain the status and PID
of child procsses. A parent process can create a child process and have it do
some work and wait for it to terminate and also check the exit status of the
child process. Therefore, the process descriptor of the child process cannot be
discarded right after process termination. It is only allowed after the parent
has issued a `wait()` like syscall that refers to the terminated process or if
the parent dies. This is why the `EXIT_ZOMBIE` process state exists. If the
parent process is terminated before the child, the orphaned processes are
assigned to become children of the `init` process. The `init` process
continuously calls `wait()` on its children to destroy these zombie processes.
The `release_task()` function is responsible for detaching the data structures of
a process. It is invoked from either `do_exit()` in which memory reclaiming is
done by the scheduler or from a `wait()` like syscall in which memory is
reclaimed in the `release_task()` function
