# Copy Swap Idiom

The C++ core guidelines states that a swap function should not fail and be
`noexcept`. Failure in this case refers to guarantees that a function makes in
regards to the exceptions it can throw. The different types of guarantees are
as follows:
- No throw: The operation is guaranteed to succeed and satisfy all requirements
without throwing exceptions to the caller
- Strong safety: Operations can fail but failed operations are guaranteed to
have no side effects
- Basic saffety: Partial execution of failed operations can cause side effects
but invariants of the program are preserved and there are no resource leaks
- No safety: No guarantees are made

In C++, for a type to be regular, meaning it behaves like the built-in types
like `int` and works with the STL, it must support the swap idiom. Consider the
object below:
```C++
class Foo {
public:
    void swap(Foo& rhs) noexcept {
        m1.swap(rhs.m1);
        std::swap(m2, rhs.m2);
    }
private:
    Bar m1;
    int m2;
};
```
It supports a swap function. This swap function should not throw exceptions as
well. Swap functions should not fail.

You should also support a non-member swap function as follows
```C++
void swap(Foo& a, Foo& b) noexcept {
    a.swap(b);
}
```

Otherwise, the STL will default to the `std::swap` function which looks like
```C++
template<typename T>
void std::swap(T& a, T& b) noexcept {
    T tmp(std::move(a));
    a = std::move(b);
    b = std::move(tmp);
}
```

The copy and swap idiom can be used to implement the copy and move assignment
operators as shown with the `Example` class. In both cases, it first makes a
temprorary object relying on the copy and move constructors. In this case, it
is `default` by the compiler in which case the move constructor will be
`noexcept` if all children's move constructors are `noexcept`. The C++ compiler
is capable of generating these defeault constructors by doing member variable
wise copies or moves. As long as you don't implement a custom destructor or any
custom copy or move constructor/operator, you can use the default ones.

Once the temporary copy is made, the swap function is used. As long as the swap
functions are noexcept, the copy and move assignment operators are also
noexcept. This allows you to do transaction style operation where you first
create a copy to work and then swap the results once everything is okay.
