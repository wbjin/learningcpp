# Weak Pointers

A problem with shared pointers are cyclic references. Because shared pointers
track the number of references to an object an only deallocate once the
reference count is 0, you can have situations like below:
```C++
struct B;  // forward declaration

struct A {
    std::shared_ptr<B> b;
    ~A() {
        std::cout << "A destroyed\n";
    }

};

struct B {
    std::shared_ptr<A> a;
    ~B() {
        std::cout << "B destroyed\n";
    }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b = b;
    b->a = a;
}
```

Neither `a` or `b` will be dellocated because at the end of main, both have a
reference count of 2.

The solution to this is to use weak pointers instead. Weak pointers are
essentially raw pointers execpt for the fact that `delete` cannot be called on
the pointer. It does not have any ownership of the object and does not
contribute to the reference count. This means that it is possible for a weak
pointer to be pointing to nothing. To protect from nullptr dereferencing, the
weak poiner exposes a `lock` method that creates a shared pointer out of a weak
pointer. It first checks if the weak pointer is still pointing to a valid
object and then upgrades the pointer to a shared pointer to ensure that the
object is not deallocated. `lock` will throw an exception if it is pointing to
an invalid object and needs to be checekd with `expired` or `use_count`
methods.

```C++
struct B;

struct A {
        std::shared_ptr<B> b;
};

struct B {
        std::weak_ptr<A> a;   // does not contribute to ref count
};
```
