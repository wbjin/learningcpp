# Size of a Class

The factors that determine the size of a class are
- Size of non static data members: Static member variables are not part of a
class instance and is thus not a part of the size of the class
- Order of non static data members
- Byte alignment
- Size of immediate base class
- Existence of virtual functions: Will add `sizeof(void*)` to the class because
it needs a virtual table pointer.
- Compiler
- Mode of inheritance

## Byte alignment

```C++
class C { 
    char c; 
    int int1; 
    int int2; 
    int i; 
    long l; 
    short s; 
}; 
```

The order in which you declare non static member variables matter because of
something called byte alignment. The rule is, an object must start at a
multiple of its type's alignment (`alignof`). Typically, the alignment of an
object is exactly the size of the object, ie the alignment of `int32` is 4 and
`char` is 1. For a user defined type, the alginment is the maximum alignment of
all of its non stsatic members.

For example, assume `C` starts at memory address `0x0`. `c` will be in address
`0x00`. Because the alignment of `inti` is 4, the compiler will add padding to
make it start at `0x04`. This allows effcient hardware accesses. A rule of
thumb is to order the data members of an object in biggest to smallest order.

Additionally, the compiler will also add padding to the tail of the
class/struct (after the last member) to make the size of the object a multiple
of the alignment of the object.

For object of type `C`, the memory layout would look like
```
class C
┌───────────────┬──────────────┬──────────────┐
│ Offset        │ Field        │ Size         │
├───────────────┼──────────────┼──────────────┤
│ 0             │ c            │ 1 byte       │
│ 1 - 3         │ padding      │ 3 bytes      │
│ 4 - 7         │ int1         │ 4 bytes      │
│ 8 - 11        │ int2         │ 4 bytes      │
│ 12 - 15       │ i            │ 4 bytes      │
│ 16 - 23       │ l            │ 8 bytes      │
│ 24 - 25       │ s            │ 2 bytes      │
│ 26 - 31       │ tail padding │ 6 bytes      │
└───────────────┴──────────────┴──────────────┘
```
The alignment of `C` is 8 (because the `long l`) so the size must be a multiple
of 8 which in this case is `32`.

## Size of immediate base class
```C++
struct A {
    int a;
};

struct B : A {
    int b;
};

struct D : B {
    int d;
};
```

The size of `D` is `sizeof(B)` plus the data members of `D` including the
padding needed for alignment. Remember that `sizeof(B)` also includes A. If `D`
was misaligned after `B`, there would be padding added in between the last data
member of  `B` and start of `D`.
