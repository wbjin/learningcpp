# The UNIX Time Sharing System

## Summary
UNIX is a multi-user, interactive operating system with a hierarchical file
system, file, device, and inter process I/O, asynchronous processes, system
command language, and many subsystems for different peripherals.

## Introduction
The system described in the paper is the third version of UNIX used for things
like text editing, data processing, and computer science reasearch in OS,
networks, and programming languages. The most important point that this paper
intends to make is that it is possible to create a powerful operating system
with neither expensive hardware or extensive human effort while also delivering
novel features that aren't found in other systems

## Hardware and Software Environment
The main different in hardware for UNIX compare to other systems of the time is
the "generous allotment" of I/O buffers and system table (memory?) as well as a
lot of different peripherals and device drivers. One interesting thing is that
most of the system is written in C. While earlier versions were written in
machine language, it was rewritten in C because it allows for easier editing
and adding and also functionality improvements.

## File System
From the user's POV there are three kinds of files: ordinary disk files,
directories, and special files.

Ordinary files contain user specified information like human readable strings
or executable code.

Directories maintain the mapping between file names and the file contents.
Directories can also contain mappings to other subdirectories. Directories can
only be edited by the system through users with the permissions required. The
file system is a tree structure with the root being the "root" directory. All
files and directories are descendants of this root directory. The path names
specify the location of a file or directory. Path names are names of
directories separated by a "/" with the absolute path starting from the root
directory "/alpha/beta/gamma" and a relative path starting from the directory
that the user is in "beta/gamma". Linking is when directories or files are
referenced with different names. This is possible because files aren't part of
a directory, directories only contain names to files so it is possible for
multiple directories to contain links to the same file. Each directory has at
least two entries: "." which refers to itself and ".." which refers to its
parent directory. Directories are only permitted to be part of one parent to
avoid cycles (no hardlinking directories, only soft linking) (hard links point
to the actual data on disk while soft links are just aliases for different
paths).

Special files are for I/O devices where each I/O device is associated with at
least one special file. Special files are read and writable but these read and
writes activate the devices that are associated with the file. Some specifal
files are protected from user access such as disks and core files. Accessing
I/O devices this way makes it possible to interchange files and I/O device
special files. It also allows protection of special files just like normal
files.

Because the file system is treated like a tree, it is possible to have a
subtree stored on another device (the root always has to be on the host
machine). The mount system request takes in the name of an ordinary file and
the name of a special file used associated for directory access through a hard
drive. Through this disk access specifal file, the host machine can access the
file system tree in the separate hard disk. Any references to the original
ordinary file is now a reference to the root of the file system in the attached
hard disk. The directories and files in this separate drive is treated the same
way as any other directories and files in the system with the exception that
links cannot exist across file systems. This is to avoid having to remove links
when a file system is removed.

Each user is assigned an user id and a file created by a user will be marked
with their user id. Files have seven protection bits, 6 of these for read,
write, and executable permissions for the user that created it and all other
users. The sevent bit allows the current user to act as the user that created
the file only during execution of the file. This is useful when set in
executable programs as it allows the program to edit files while the user that
invoked the program may not be able to directly edit the file. This can be
usefuly in situations where there is a program owned by the root user but
beecause the sevent bit is set, any user is able to access and execute the
program. An execption to this protection mechanism is the super user that has
unfiltered access to the entire file system.

I/O calls don't differentiate the devices or the style of access (sequential or
random access). There is no set size limit for a file and the size is the
highest byte written to that file. Files are opened with a `open(name, flag)`
call that returns a file descriptor integer that is used to refer to the file
in subsequent calls. The `create` call allows users to make a new or completely
rewrite a file. There are no limits to how many processes can have a file open
and there are no mechanisms to prevent simultaneous access to a file via a
lock. `read(fd, buffer, count)` and `write(fd, buffer, count)` can be used to
read and write to a file sequentially where for each file, there is a pointer
to pointing to the next byte to read or write (this pointer is managed by the
system). The return value fo these functions are the number of bytes read or
written and will be the same as count execpt cases where there are errors or
you reached the end of the file. `seek(fd, base, offset)` can be used to move
the internal pointer to a sepcific location for random access. Base can be any
location in the file and offset can be negative or positive. It returns the
actual offset from the beginning of the file that the pointer ends up at.

## Implementation of the File System
A directory only contains the name of a file and the pointer to the file
contents. This pointer is an integer called the i-number where this i-number is
used as a key to the system table which has the location of where the file is
stored in disk. This is used to find the i-node of an entry which contains the
owner, protection bits, physical disk or tape addresses for the file, the
size, time of last modification, number of links to file, bit indicating
whether it is directory or file, bit indicating special file, bit indicating
large or small file. Disks are partitioned into 512 byte blocks and inodes have
space for 8 addresses of the blocks themselves or addresses to indirect blocks
that contain 256 addresses. 

## Processes and Images
Image is a computer execution environment that includes a core image,
registers, open files, and other metadata. A process is an execution of an
image where a processor executing a process must have the image in memory until
it is swapped out. The memory of an image is split into the text segment
containing the code, the heap starting from an 8K byte offset from address 0,
and the stack that grows downwards from the highest virtual address.

Processes are created only with the `fork()` call (except during bootstrap)
which creates two independent processes that share the same memory except for
the process id that is returned by `fork()`. 

Pipes are used for communication between processes. The `pipe()` call returns a
file descriptor that can be read and written to and this fd is passed to the
parent and the child on a fork.

Execution of a program is done with `execute(file, arg1, arg2, ...)` that
executes the file located at file. Unless `execute` errors, it will never
return

`wait()` can be used to synchrnoize processes. The calling process will not
return from the call until the process it is waiting on exists or there is an
error.

`exit()` can be used to terminate a process. It destroys the image.

## Shell
The shell reads input from users and executes it on their behalf. 

Program started by the shell have two file descriptors, 0 and 1 for standard
input and standard output

## Traps
The processor detects faults such as referencing nonexistence memory. The
processor traps to a system routine and the routeine is responsible for
handling the trap. Typically the routine will terminate the violating process.
There are also interrupts that allow a user to terminate a process and remove
the image from the shell. The quit command will do the same but produce a core
dump for inspection.
