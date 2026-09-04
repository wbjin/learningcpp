# Virtual Functions

A virtual function is a member function that resolves to the most derived
version of the function when called, even if you don't have a pointer or
reference of the derived type. You can also think of a class with a virtual
function as being the root node of a polymorphism tree.

```C++
class A { public: void f() { std::cout << "A\n"; } };

class B : public A { public: virtual void f() { std::cout << "B\n"; } };

class C : public B { public: void f() override { std::cout << "C\n"; } };
```

In the above case, `f()` is only polymorphic starting from `B`.
```C++
C c;

A* ap = &c;
B* bp = &c;
```
`ap->f()` will result in A and `bp->f()` will result in C.

**Never call virtual functions in constructors and destructors**. If you were
to call a virtual function from a base class, the derived class would not be
instantiated yet so you would be calling a method on a class that has not been
insantiated. It will fall back to the base class function. Similar for the
destructor because the derived class instance would be destructed already.

## `override` and `final`
If a method in the derived class does not have the exact same signature as the
base class that defines the virtual method, runtime polymorphism will not work.
For example, if the function arguments are different if the constness of the
method is different, the base class method will be called instead of the
derived class method.

The `override` keyword can be used to tell the compiler to check that the
signature matches exactly.

```C++
class A
{
public:
	virtual std::string_view getName1(int x) { return "A"; }
	virtual std::string_view getName2(int x) { return "A"; }
	virtual std::string_view getName3(int x) { return "A"; }
};

class B : public A
{
public:
	std::string_view getName1(short int x) override { return "B"; } // compile error, function is not an override
	std::string_view getName2(int x) const override { return "B"; } // compile error, function is not an override
	std::string_view getName3(int x) override { return "B"; } // okay, function is an override of A::getName3(int)

};
```

In order to stop a class from being derived from or to stop a method from being
overriden further, you can use the `final` keyword.

To mark a method as final:
```C++
class A {
public: virtual std::string_view getName() const { return "A"; }
};

class B : public A {
public:
    // use of final
	std::string_view getName() const override final { return "B"; } // okay, overrides A::getName()
};

class C : public B {
public:
	std::string_view getName() const override { return "C"; } // compile error: overrides B::getName(), which is final
};
```

To mark a class as final:
```C++
class A {
public:
	virtual std::string_view getName() const { return "A"; }
};

// use of final
class B final : public A {
public:
	std::string_view getName() const override { return "B"; }
};

class C : public B // compile error: cannot inherit from final class
{
};

```
The `final` can also be used for devirtualization, a way to make virtual
functions resolve at compile time.

```C++
struct base { 
  virtual void f() const = 0; 
}; 
 
struct derived final : base { 
  void f() const override {} 
};

void call_f(derived const& d) { 
  d.f(); 
}
```
In the case above, the compiler knows that `derived` cannot be futher inherited
from and thus `derived::f` is the derived most function. This way, the call to
`d.f` can be resolved at copmile time. Consider marking your classes as `final`
if you don't plan on inheriting from it.

## Covariant Return Types
A virtual function in a derived class can have a different return type than the
base class function if the returned type is a pointer or reference to a class
that is derived from the base class.

```C++
class Animal {
public:
    virtual Animal* clone() const {
        return new Animal(*this);
    }

};

class Dog : public Animal {
public:
    Dog* clone() const override {
        return new Dog(*this);
    }

};
```
This is because `Dog*` is convertible to `Animal*`

An important thing to note is that the return type will depend on the type of
the pointer or reference that the method is called from. In the following snippet

```C++
Dog d {};
Animal* a {&d};

d.clone(); // returns Dog*
a->clone(); // calls the derived `clone` (object is actually a Dog) but returns `Animal*`
```
This is because the derived `Dog*` is upcast to an `Animal*` implicitly.

## Virtual destructors

When dealing with classes with any virtual method, you need to make the
destructor in your base class virtual as well. This is becasue when destructing
the derived class through a base class pointer, we want to make sure to call
the derived class's destructor but without marking the base class destructor as
virtual, the base class destructor will be called instead of the derived class
destructor.

A good way to protect against leaking resources with polymorphism is to either
1) mark your destructor as virtual and public if it is meant to be derived from
or 2) mark the class as final so no other class can derive from it.

The only situation where you don't need to mark the destructor as virtual is
when using `shared_ptr` (though it is still advised). During the creation of a
`shared_ptr`, the original type of the pointer and thus its original destructor
is captured. This would only work if the `shared_ptr` was created directly from
a derived class `auto ptr = std::make_shared<Derived>()` and not `auto ptr =
std::make_shared<Derived>(ptr_type_base)`

## Virtual assignment

Typically, you do not want to virtualize the assignment operator. Consider the
below case, 
```C++
Dog dog;
Cat cat;

Base& lhs = dog;
Base& rhs = cat;

lhs = rhs;
```

`rhs` at runtime is a `Dog` type so the assignment operator for `Dog` will be
called. However, it is not well defined in this case how to copy a `Cat` object
into a `Dog` object. Thus it is recommended to make the base copy and move
operations (assignment and constructors) protected and non-virtual or outright
deleted. Instead, derived classes should implement a `clone` method that allows
the duplication of a polymorphic object through a bae pointer.

```C++
struct Base {
    virtual std::unique_ptr<Base> clone() const = 0;
    virtual ~Base() = default;
};

struct Derived : Base {
    int x{};

    std::unique_ptr<Base> clone() const override {
        return std::make_unique<Derived>(*this);
    }
};
```

## Early and late binding

Function binding is the process of determining which function definition is
associatd with a function call (because a function identifier/name can have
many different function definitions it can dispatch to).

Early/static binding refers to function calls that are resolved at compile
time. The linker is able to generate machine code that tells the CPU to jump to
the exact address that the function lives in. Calls to regular functions,
overloaded functions, and template functions can be resolved at compile time.

Late binding is when a function call is resolved at runtime. One way to get
late binding is through function pointers where a variable points to the
location of a function rather than a variable. In this case, the compiler is
unable to determine which function to jump to at copmile and must access the
memory associated with the function pointer to figure out where the function to
jump to lives.

## Virtual table

The C++ standard does not specify how to implement virtual functions. Howerver,
most implementations use something called the vtable to do runtime dispatch.
Every class that has at least one method with the `virtual` keyword or inherits
from a class with a virtual function contains a vtable. The vtable is simply a
static array of function pointers that is setup by the compiler. The static
array contains pointers to the most derived function accessible by that class.

The compiler will also add a hidden member variable to the class called
`__vptr` (name depends on impl) which points to the vtable. The `__vptr` is
also inherited by derived classes.

In the following example:

```C++
class Base {
public:
    virtual void function1() {};
    virtual void function2() {};
};

class D1: public Base {
public:
    void function1() override {};
};

class D2: public Base {
public:
    void function2() override {};
};
```

the compiler adds something like `VirtualTable* __vptr;` as a member of `Base`.
The value of `__vptr` for each class (the vtable that it actually points) to
depends on the class, ie each class has a unique vtable. The vtable contains
function pointers to the implementation of each virtual function in that class.
For `Base`, it is:

```
Base vtable
+----------------------+
| &Base::function1     |
+----------------------+
| &Base::function2     |
+----------------------+
```

For `D1` it is:

```
D1 vtable
+----------------------+
| &D1::function1       |  // overridden
+----------------------+
| &Base::function2     |  // inherited
+----------------------+
```
 and for `D2` it is:
```
D2 vtable
+----------------------+
| &Base::function1     |  // inherited
+----------------------+
| &D2::function2       |  // overridden
+----------------------+
```

So when we call something like this:
```C++
D1 d1 {};
Base* dPtr = &d1;
dPtr->function1();
```

`dPtr` is pointing to the `Base` portion of `d1`. This `Base` portion contains
a `__vptr` that points to the `vtable` of `D1`. The compiler knows that
`function1` is a virtual function so it looks into the `vtable` and looks up
which function it actually resolves to (in this case, `D1`'s implementation).

Calling a virtual function this way incurs three additional loads. First to
load the `__vptr`. Second to index into the vtable with the `__vptr`. Third to
jump to the correct function address.

## Pure Virtual Functions, Abstract base class, Interface class

A pure virtual function is a virtual function in the base class with no body.
It is denoted with a `= 0` keyword.
```C++
class Base
{
public:
    virtual int getValue() const = 0; // a pure virtual function

    int doSomething() = 0; // Compile error: can not set non-virtual functions to 0
};
```

This means that any class that inherits from the base class must implement this
virtual function (leaving it unimplemented will make the derived class an
abstract base class as well). Any class with even one pure virtual function
becomes an abstract base class which means no instance of the base class can be
created directly.

It is possible for a pure virtual function to still have a function body.

```C++
class Animal // This Animal is an abstract base class
{
public:
    Animal() = default
    virtual ~Animal() = default;
    virtual std::string_view speak() const = 0;

};

std::string_view Animal::speak() const  // even though it has a definition
{
    return "buzz";
}
```

This still forces derived classes to implement the function but it allows it to
fall back to a default implementation if desired.

```C++
class Dragonfly: public Animal
{
public:
    Dragonfly() = default;

    std::string_view speak() const override
    {
        return Animal::speak(); // use Animal's default implementation
    }
};

```

An interface class refers to a class where all methods are pure virtual and
there are no member variables. This class defines an interface that all derived
classes must implement.

## Virtual base classes

The diamond problem is a classic problem that arises in inheritance. Consider
the following scenario

```C++
struct A {
    int x {};
};

struct B : A {};
struct C : A {};

struct D : B, C {
    void func() {
        std::cout << x;
    }
};
```

In the above case, the layout in memory of `D` would be something like

```
D object
┌────────────────────┐
│ B subobject        │
│   A subobject      │
│     int x          │
├────────────────────┤
│ C subobject        │
│   A subobject      │
│     int x          │
└────────────────────┘
```

So what is the `x` in `func` referring to. It is ambiguous because there are
two instances of `x.` While this may be desired sometimes, in order to have
just one base class object, the `virtual` keyword in the inheritance list is
required.

```C++
struct A {
    int x {};
};

struct B : virtual A {};
struct C : virtual A {};

struct D : B, C {
    D(int x) : A{x} {}

    void func() {
        std::cout << x;
    }
};
```

The memory layout looks something like
```
D object
┌────────────────────────────┐
│ B subobject                │
│   hidden virtual-base info │
├────────────────────────────┤
│ C subobject                │
│   hidden virtual-base info │
├────────────────────────────┤
│ shared A virtual base      │
│   int x                    │
└────────────────────────────┘
```

The virtual keyword in the inheritance list tells the compiler ot share an
instance of hte base class. Note that both `B` and `C` must have the `virtual`
keyword in the inheritance.

Note that in the case of using virtual inheritance, the most derived class is
responsible for manually calling the constructor of the base class, ie `D` must
call the constructor of `A` manually. Make sure to call the constructors of the
classes in order of inheritance, ie the most base class first to the derived
classes.

## Misc
Default parameters are taken into consideration at compile time while dynamic
dsipatch to the correct function happens at runtime. This can mean that a
default parameter defined for a polymorphic function may not be what you
expect.

```C++
struct C {
    virtual void foo(int a = 1) {
        std::cout << "C" << a;
    }
};

struct D : C {
    virtual void foo(int a = 2) {
        std::cout << "D" << a;
    }
};

int main () {
    C* d = new D;
    d->foo();
}
```

In the code snippet above, the output is D1 because you are calling `foo`
through a `C*` which means the default parameter `a` has a value of 1.
Howerver, the function is dispatched at runtime to `D`'s implementation of
`foo`.
