# Abstract Factory Design Pattern

Compile with
```bash
g++ main.cpp -std=c++20-o main
```

You are used to polymorphism where there is a base type/interface, ie a base
class and many different subtypes, ie inherited classes that implement the same
interface differently. For example, `GCPStorageController` and
`AWSStorageController` implement the same interface, `StorageController` but
for different cloud providers with different implementations.

Often times, there are families of these inherited class
that are meant to work together. For example, the `GCPStorageController` should
always be used with `GCPComputeController` and the `AWSStorageController`
should always be used with `AWSComputeController`. These should not mix.

When there are many families of these subtypes, making manual allocations can
lead to mismatch in the subtypes.

The abstract factory pattern abstracts this away by creating a factory per
family of subytpes, ie `AWSControllerFactory` and `GCPControllerFactory`. These
factories for each family are derived off a common `ControllerFactory` base
class to provide a common interface but to also make it so that when using the
actual derived factories, you don't have to worry about which factory it
actually is.

When using the abstract factory pattern, the factory only needs to know the
base type, for example `factory` in `main` is a unique pointer of type
`ControllerFactory`. You don't need to change the type of `factory` when
changing the underlying factory implementation.

### C++ Notes
You have to inherit with `public BaseClass` for a pointer of the derived class
to be implicitly convertible to a pointer of the base class.

When creating a pure virtual class, you have to mark the destructor as virtual
so that when you `delete` based on the base class pointer, you invoke the
correct derived class destructor. With the virtual destructor marked, it first
runs the derived class destructor and then the base class destructor. Without
the destructor being marked virtual, only the base class destructor runs which
is UB.
