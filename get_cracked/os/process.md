# The Process

## Stack and Frame Pointers
The stack pointer points to the top of the stack, ie where the next value will
be pushed or popped from. The frame pointer points to the start of the current
function's stack frame. It is used as a reference point to access function
parameters, arguments, and local variables. `rsp` is the stack pointer and
`rbp` is the frame pointer (base pointer) in x86-64.

Consider the C code below
```C
void callee(int x) {
    int y = x + 1;
}

int main() {
    callee(5);
    return 0;
}
```

To call `callee`, something like this would be executed
```x86asm
push 5
call callee
```
At this point, the stack looks something like
```
┌───────────────┐
│ main's SF     │
├───────────────┤
│     5         │  ← argument x
├───────────────┤
│ return addr   │  ← pushed by call
└───────────────┘
        ↑
       rsp
```
The `call` instruction pushes the address of the next instruction onto the
stack. `rsp` (stack pointer) always points to the top of the stack. Currently,
the `rbp` (base pointer) is still pointing to the start of the stack frame for
main.

This is the calling convention of a particular ABI (in this case x86-64 on
Linux). In x86-64, there are 6 registers for function arguments. Anything else
will be placed in the stack. Arguments that are fit in a single machine word
(essentially the register sizes in a machine) take up one register. Arguments
that take up two to four machine words span sequential registers. Anything
bigger is placed on the stack.

Then, `callee`'s "prologue" is executed.
```x86asm
push rbp        ; save caller’s frame pointer
mov rbp, rsp    ; set new frame pointer
sub rsp, 16     ; allocate space for locals
```
At this point, the stack looks something like
```
┌───────────────┐
│ main's SF     │
├───────────────┤
│     5         │  ← argument (x)
├───────────────┤
│ return addr   │
├───────────────┤
│ old rbp       │  ← saved FP, rbp pointing here
├───────────────┤
│   (space)     │  ← local y
└───────────────┘
        ↑
       rsp (moved down)
```
The base pointer of `main` is pushed onto the stack. Before `rsp` is updated,
`rbp` is set to the current `rsp` which is where the old `rbp` lives. Then,
`rsp` is updated and the stack of `callee` is expanded enough to accomodate the
local variables.

If `callee` wants to access variables, it can use `rbp` to access it.
```x86asm
mov eax, [rbp + 16]   ; x (argument)
mov [rbp - 4], eax    ; y (local)
```

When the function returns, the function "epilogue" is run.
```x86asm
mov rsp, rbp    ; deallocate locals
pop rbp         ; restore caller FP
ret             ; pop return addr
```
By moving `rsp` to the base of the current function (`callee`), memory that was
allocated for `callee` is considered free and usable (though the contents still
remain). After resetting the `rsp`, it now points to the `rbp` of the calling
function (`main`). This is popped into the `rbp` register and `rsp` is updated
as well. After the `pop`, the `rsp` points to the return address that was added
to the stack as part of the `call callee` instruction. `ret` moves the
instruction pointer to the return address and updates `rsp` as well.

If the function has a return value, it is placed in a register for return
values. If the return value is bigger than a machine word, then the return
value register instead has a pointer to a memory region allocated by the
caller. The `callee` puts the return value in this memory region.

## Thread Joins
When a thread finishes its execution, it's resources are still kept. The TCB,
stack memory, and exit status is kept around until a different thread calls
`pthread_join` or unless the thread is started in detached mode.
