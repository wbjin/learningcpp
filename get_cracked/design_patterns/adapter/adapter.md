# Adapter Pattern

The adapter pattern allows objects with incompatible interfaces to work
together by sitting in between the incompatible interfaces. It is useful when
you have an existing abstraction that you want to use but is incompatible with
the rest of your code. In this case, instead of changing the abstraction (which
you may not be able to if you don't have the source), using an adapter allows
you to use the abstraction. Furthermore, if you have many subclasses that you
want to use but is not incompatible, instead of implementing the same thing
multiple times, you can use an adapter that works with any of your subclasses.
Often times you may not even want to change the interface of the abstraction
because other parts of the codebase depend on it.

For example, we have `LoggeA` and `LoggerB` in our program. A is a more modern
logging system while B is a legacy logging system that we still need to
support. The two have different interfaces but we want to use them as if they
have the same interface.

`LoggerAdapter` defines a base class interface for logging regardless of the
underlying implementation. `LoggerAAdapter` and `LoggerBAdapter` inherit from
the base class and translate the input to match the interface provided by the
underlying logging system. This allows the application to use both loggers
through the unified `LoggerAdapter` interface.

## C++ Notes
The order of inheritance dictates serveral things in C++. If you inherit 1) `class
C : public A, public B` vs 2) `class C : public B, public A`, it effects
- Construction order: In 1) A -> B -> C. In 2) B -> A -> C. Naturally, this
dictates destruction order as well
- Memory layout: in 1) A contains B which contains C, ie A comes first in
memory. In 2) B contains A which contains C. When you have virtual functions,
this affects things like the location of the `vptr`, `vtables` and the padding
of the classes

You can have multiple references to a `std::unique_ptr`. When you iterate
through a container of unique pointers, you can use the `cosnt auto&` idiom to
avoid copies or moves.
