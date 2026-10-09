# Storage Durations

C++ has four different storage durations that determines how long the memory
for an object exists: automatic, static, thread, and dynamic.

## Automatic Storage Duration
This is the lifetime of stackc variables and function parameters. The memory
for an object exists for the enclosing block's execution.

## Static Storage Duration
The memory lasts for the entire duration of the program and applies to
variabels with the `static`.

Destruction order follows reverse initialization order. Initialization happens
when execution first encounters the variable.

The order in which static globals are initalized in the same translation unit
is well defined. It happens in definition order. However, the order is not well
defined across different translation units.

## Thread Storage Duration
A variable declared with `thread_local` ensures that the memory exists for the
lifetime of the thread. Changing one instance in one thread does not affect
other threads.

## Dynamic Storage Duration
The program explicitly allocates and deallocates objects in memory.
