# Constructor and Destructors

```C++
struct A {
    A() {std::cout << "A()";}
    ~A() {std::cout << "~A()";}
};

struct B {
    B() {std::cout << "B()";}
    ~B() {std::cout << "~B()";}
    A a;
};

struct C : A {
    C() {std::cout << "C()";}
    ~C() {std::cout << "~C()";}
};


int main() {
{
    B b;
}
    std::cout << std::endl;
{
    C c;
}
}
```
This prints
```txt
A()B()~B()~A()
A()C()~C()~A()
```

In both cases where an object is a member or is inherited from, that object's
constructor is called before the object you that is embedding or inheriting.

Object construction happens in the order of 1) virtual base 2) base 3) members
4) object being constructed.

## Constructor Template
Templated copy constructor/operator and move constructor/operator do not
supress implicit declaration of these members. During overload resolution, a
non-templated match wins over a templated match so the implicitly generated
declarations would match. For example:
```C++
template <typename T>
class Object {
    public:
        Object(int) {};

        template <typename U>
        Object(U) {};

        template <typename F>
        Object(const Object<F>&);

        template <typename G>
        Object(G&&);
};
```
- Between the ctors `Object(int)` and `Object(U)`, the first will get matched
first during resolution if being constructed with an `int`. If it does not
match the parameter type, it will be matched with the template specification.
- The construtor template `Object(const Object<F>&)` does not supress implicit
declaration of the copy constructor `Object(const Object&)`. Constructing from
another object of the same template specification will match with the implicit
copy constructor.
- `Object(G&&)` is also a constructor template that take doesn't stop the
implicit declaration of the move constructor.

## Exception in Constructor
If there is an exception in the constructor body, the object will not be fully
constructed and its destructor will not be called. Already constructed bases
and members will be destructed and the exception will propagate to a catch.

## Copy Elision and Named Return Value Optimization
Since C++17, fucntions that return objects can be optimized to not use a move
or copy, instead it can be created in the final destination.
```C++
Object make() {
    return Object(3);
}

Object temperature = make();
```
This creates `temperature` without any move or copies, instead the constructor
is called directly into `temperature.`

Named return value optimization extends this to functions that return objects
as variables.
```C++
Object make() {
    Object obj {};
    // ...
    return obj;
}

Object temperature = make();
```
This can create `Object` directly into `temperature`. NRVO is not guaranteed.

## Destructor Exceptions
Destructors are implicitly `noexcept` meaning any exception immediately turns
into a `std::terminate`

You can make a destructor `noexcept(false)` but if an exception is thrown
during the unwinding of another exception, it is a `std::terminate`.

```C++
struct X {
    X() {
        throw std::runtime_error("Constructor throw.");
    }
    ~X() {
        throw std::runtime_error("Destructor throw.");
    }
};

int main() {
    std::cout
        << (std::is_nothrow_constructible_v<X> ? 'y' : 'n')
        << (std::is_nothrow_destructible_v<X> ? 'y' : 'n');
}
```
This prints `ny`
