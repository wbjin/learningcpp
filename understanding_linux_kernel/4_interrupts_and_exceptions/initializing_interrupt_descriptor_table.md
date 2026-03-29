# Initializing the Interrupt Descriptor Table

Before enabling interrupts, the kernel must load the address of the Interrupt
Descriptor Table into the `idtr` register. Users can fire interrupts in user
mode with the `int` instruction along with any interrupt vector (0-255).
However, some interrupt vectors must be disabled from user mode which can be
done with fields in the entries of the IDT. 

## Interrupt, Trap, and System Gates
Intel provides three types of interrupt descriptors that can be in the IDT:
Task, Interrupt, and Trap Gate Descriptors. Linux uses the following
classifications. A "gate" is essentially an IDT entry and it tells the CPU how
to transfer control to the handler

- Interrupt gate: An Intel interrupt gate that can't be accessed in user mode.
All Linux interrupt handlers are activated by interrupt gates and are all
restricted to kernel mode. All hardware interrupt handlers and critical
exceptions uses this gate type. It also disables maskable interrupts on entry
- Trap gate: Does not clear the IF flag so interrupts can be fired during it.
Only accessible in kernel mode and is used for most exception handlers like
page faults. 
- System gate: Can be invoked from user mode. 
- System interrupt gate: Can also be invoked from user mode but also clears IF
flag so no maskable interrupts during the handling.
- Task gate: Causes hardware tasks witch via the TSS. Used for double fault
handling which can occur when an exception happens while handling another
exception (which shouldn't happen)

## Preliminary Initialization of the IDT
The IDT is initialized by the BIOS routines while the computer operates in real
mode. Once the kernel takes over, IDT is moved to another region of RAM and
initialized a second time. During kernel initialization, the setup fills all
entries of the IDT with an ignore interrupt handler which saves the contents of
the registers onto the stack, invokes `printk` to log "Unknwon interrupt",
restores the register contents, and exceutes `iret` to restart the interrupted
process. After this preliminary initialization, the kernel makes a second pass
and replaces the null handlers with the actual trap and interrupt handlers.
