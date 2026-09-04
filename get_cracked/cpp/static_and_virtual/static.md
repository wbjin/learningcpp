# Static Keyword

The `static` keyword on a symbol sepcifies that the lifetime of the symbol
lasts during the entirety of program duration.

## In functions

Static variables in functions retains its value through the entirety of the
program across multiple function calls. The static variable is initialized the
first time code execution reaches its declaration and is thread safe since
C++11 meaning if two threads access the declaration at the same time, one will
wait. The destructor of the static variable will be called after main exists.
Static variables live in the data segment of the program image (or the BSS
segment if it is not initalized).

The copmiler implements static storage duration via a hidden guard value. In an
example like this
```C++
void foo() {
    static int x = make();
}
```

the compiler creates code that looks something like
```
foo:
    cmp     byte ptr [guard_for_x], 0
    jne     .already_initialized

    call    make
    mov     dword ptr [x], eax
    mov     byte ptr [guard_for_x], 1

.already_initialized:
    ret
```

where memory looks something like
```
.bss
x:
    .zero 4

guard_for_x:
    .zero 1
```

The first time code execution reaches the static variable, there is a check for
the guard value. If the guard value is not set, the static variable is
initialized and the guard value is set. In future function calls, the
initialization is skipped. Note that this is a simplified example and in
actually C++, there would be something like a lock guard acquired before and
realeased after the initialization.

## In global scope
A global variable with static is internally linked meaning it is only usable
within the file. In order to access it, you need to implement getters and
settters.

Global static variables are initialized before main begins. There are two ways
they can be initialized. If the variable is a constant or a zero, it is
initialized at program load time. If the global static variable has a zero
value, it is in the .bss section, otherwise it is put in the data section. For
global statics that have dynamic initialization (computation is required), the
compiler inserts initialization code that runs before main.

Within a file, static initialiation occurs in declaration order. Howerver,
across translation units, there is no guarnatee in which the order of static
initialization occurs.

## In namespace
A static variable in a namespace behaves the same way as a static variable in
global scope. It can only be referenced within the file

In mordern C++, anonymous naemspaces are perferred over static globals.

```C++

namespace {
const int global1 = 1; // preferred over static
}
static int global2 = 2;
```

## In classes
A static member variable in a class belongs to the class and not any instance
of the class. Historically, the definition of the static member variable needed
to happen outside the declaration of the class.

```C++
class Obj {
    static int s;
    inline static int s2 = 0;
};

Obj::s = 0;
```

This is because the static member is not associated with any instance of the
class and needs explicit allocation in memory which is taken care by the
definition outside of the class. Furthermore, including the class multiple
times would create multiple copies if the definition and declaration happened
together.

Modern C++ allows the use of the `inline` keyword to make sure there is only
one copy of the static member variable even if the definition happens with the
declaration.
