# Empty Base Class Optimization

The C++ standard requires that even a class with no data members must occupy
memory. Thus if you have the following

```C++
class Empty {};
```

The `sizeof(Empty)` would actually be 1.

However, C++ doesn't specify this for base classes so if you have the following

```C++
class EmptyEmpty : public Empty {};
```

The `sizeof(EmptyEmpty)` would still be 1. This is known as Empty Base Class
Optimization and is used to save spaces for interface classes.

## `[[no_unique_address]]`

A C++ 20 alternative to this is the `[[no_unique_address]]` directive. This
directive tells the compiler that a member variable does not need its own
address if it is empty.

```C++
struct Empty {};

struct X {
    [[no_unique_address]] Empty e;
    int x;
};
```

This allows the class `X` to have a size of 4 instead of 8 and is effectively
another way to do `struct X : Empty {}`.

This is used in the STL in places like `std::unique_ptr`. `std::unique_ptr<T,
Deleter>` has the option to pass in a deleter separte from `T`'s destructor. At
first glance, it would appear as if the unique pointer would have to maintain a
pointer to both the instance of the object and the custom deleter. However, by
using `[[no_unique_address]]`, an empty stateless deleter will not take up
additional space.

```C++
template <typename T, typename Deleter>
class unique_ptr {
    T* ptr;
    [[no_unique_address]] Deleter deleter;
};
```

## Stateful vs Stateless deleters

A stateless deleter is a functor that has no member variables and doesn't need
extra storage with `[[no_unique_address]]`. A stateful deleter has member
variables and requires extra storage in `std::unique_ptr`.
