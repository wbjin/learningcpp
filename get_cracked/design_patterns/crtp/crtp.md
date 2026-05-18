# Curiously Recurring Template Pattern

CRTP is a way to implement polymorphism without runtime dispatch. Essentially,
polymorphism at compile time. This is possible through the combination of
inheritance and template programming where the dervied class appears as a
template parameter in the base class:
```C++
class Child : Base<Child>;
```

One way to use CRTP is for static polymorphism. In `static_polymorphism.cpp`,
you can see that the `Derived` class exposes an interface called `interface`.
Within it, it downcasts to the derived class (either `Derived1`, or `Derived2`)
and calls the `interface_impl` function. This is possible because when calling
the `interface` function in `Base`, the actual object will always be either
`Derived1` or `Derived2` (you can't instantiate an instance of `Base`). Thus
casting to the derived class in `interface`, (stored as the template parameter
`T`) gives you access to the `interface_impl`.

`struct crtp` is a helper that makes the accessing of the underlying derived
type easier. It implements a `underlying()` method that does the static casting
to the derived class and returns it as a reference or a const reference. When
using `crtp`, the base class that defines the interface will inherit from the
`crtp` struct. The first type parameter `T` in `crtp` is what is passed into
the base class type parameter `T` (the type of the derived class). The second
is a "class template that takes one type parameter" (from chat GPT). It is a
way to refer to a templated class such as `Base`. By doing this, you get
something like `crtp<Derived1, Base>` and `crtp<Derived2, Base>`. The second
parameter is needed for `crtp` to declare `friend crtpType<T>` (effectively,
`friend Derived1` and `friend Derived2`). Notice that the `crtp` constructor is
marked as private. This along with the friend declaration is needed to prevent
compile time misuse. If we have a case of diamond inheritance, say something
like this:
```C++
template <typename T>
struct A : crtp<T> {};

template <typename T>
struct B : crtp<T> {};

class C
    : public A<C>,
      public B<C> {};
```
you get the problem where `C` contains two separate `crtp<C>` base objects.
```C++
C
 ├── A<C>
 │    └── crtp<C>
 └── B<C>
      └── crtp<C>
```
This makes `this->underlying()` ambiguous. Which `crtp<C>` is it referring to?
In order to fix this, we add a second parameter to `crtp`'s template so that we
take into account `A` and `C` in the type:
```C++
template <typename T>
struct A : crtp<T, A> {};

template <typename T>
struct B : crtp<T, B> {};

class C
    : public A<C>,
      public B<C> {};
```
The inheritance structure now becomes
```C++
C
 ├── A<C>
 │    └── crtp<C, A>
 └── B<C>
      └── crtp<C, B>
```
and there is no more ambiguity. Additionally, the private constructor and the
friend declaration in the `crtp` struct makes it so that only `A<C>` and `B<C>`
can construct their respective `crtp<C, A>` and `crtp<C, B>` instances
preventing mistakes like `struct A : crtp<T, B>`.
