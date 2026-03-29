# Exception Handling

Most exceptions from the CPU are interpreted as errors in Linux. The exception
handler sends a signal to the current process (the process that caused the
exception). However, some exceptions are used to manage hardware resources more
efficiently. For example, the page fault exception is used to defer allocation
new page frames until the last possible moment. Exception handlers have a
standard structure
1. Save contents of registers in kernel mode stck (in assembly)
2. Handle exception in C function
3. Exit handler with a special function (`ret_from_exception()`)

The double fault exception is a special case that indicates a serious kernel
misbehavior. In this case, the CPU fetches a special TSS segment descriptor
that is referred to by the task gate descriptor in the IDT. This TSS segment
resolves to a double fault exception handler in its own private stack.

## Saving the Registers for the Exception Handler
When an exception is raised, an exception entry stub written in assembly does
minimal setup before invoking the exception handler written in C. The entry
stub pushes an error code to the stack, if there is no error code, it pushes 0.
It also saves registers and processor states that may be used by the handler
onto the stack. Any exception metadata such as an error code or cause value is
placed in registers that exception handlers know about. The stub switches the
kernel to the appropriate execution context such as the correct addressing mode
and privilege. Then the higher level exception handler is invoked. This
mechanism allows the low level entry code to be architecture specific while
keeping the higher level exception handler mostly architecture independent. 

## Entering and Leaving the Exception Handler
The C exception handler stores the metadata of the exception and the exception
vector and sends a signal to the current process. The process takes care of the
signal after the termination of the exception handler and it will be handled
either in user mode if a signal handler is defined or in kernel mode. Most
exceptions will cause the kernel to kill the offending process. The exception
handler also checks whether the exception occurred in user or kernel mode. If
it was in kernel mode, it checks whether it was because of invalid syscall
arguments. Any other exception raised in kernel mode is considered due to a
kernel bug and in most cases, the `die()` function is invoked
