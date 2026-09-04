# Order of Construction

When a class inherits from another class, you can think of it as the two
classes being joined together.

```C++
class Base {
public:
    int a;        // 4 bytes
    double b;     // 8 bytes
    char c;       // 1 byte
};

class Derived : public Base {
public:
    int d;        // 4 bytes
    short e;      // 2 bytes
    char f;       // 1 byte
};

```

```
Derived object
┌────────────────────────────┐
│ Base::a        (4 bytes)   │
├────────────────────────────┤
│ padding        (4 bytes)   │  <- align double to 8 bytes
├────────────────────────────┤
│ Base::b        (8 bytes)   │
├────────────────────────────┤
│ Base::c        (1 byte)    │
├────────────────────────────┤
│ padding        (3 bytes)   │  <- align next int to 4 bytes
├────────────────────────────┤
│ Derived::d     (4 bytes)   │
├────────────────────────────┤
│ Derived::e     (2 bytes)   │
├────────────────────────────┤
│ Derived::f     (1 byte)    │
├────────────────────────────┤
│ padding        (1 byte)    │  <- object alignment
└────────────────────────────┘
```

First, the `Base` constructor is called to instantiate the `Base` member
variables. Then, the `Derived` constructor is called. This continues until
there are no more derived classes.

In the case where the class you are deriving from multiple base classes, the
order of construction goes left to right from the first class after the `:`. If
`A` was itself a derived classes, the base class of `A` would be constructed
first, then `A` then `B`.

```C++
class A {
public:
    int a;
    char b;
};

class B {
public:
    double c;
    short d;
};

class C : public A, public B {
public:
    int e;
};
```

The order of destruction is in the inverse of the order of construction. For
the above, first `C` will be destructed, then `B`, then `C`.

Something interesting is that if you were to have a code snippet that looked
something like this
```C++
int main() {
 C obj {};

  A* a_ptr = &obj;
  B* b_ptr = &obj;
  C* c_ptr = &obj;
}
```

`a_ptr` and `c_ptr` would both point to the same address, the start of the
entire object which is also the start of `A`. `b_ptr` would be pointing at the
start of `B`. This is because of the multiple inheritance. If there was only
one chain of inheritane, all the pointers would point to the same thing.
