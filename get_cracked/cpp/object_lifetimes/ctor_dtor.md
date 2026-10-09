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
