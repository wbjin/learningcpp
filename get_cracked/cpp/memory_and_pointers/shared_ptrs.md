# Shared Pointers

Typically, `std::make_shared` is the preferred way of making a shared pointer.
However, when you want to use a custom deleter, you have to use shared pointer
constructor. By default, the deleter that is used when creating a shared
pointer is the operator, delete. To use a custom deleter:

```C++
std::shared_ptr<Object> sptr = std::shared_ptr<Object>(
    new Object(), 
    [](Object* obj) {
        std::cout << "custom deleter\n";
        delete obj;
    });
```

The custom deleter is a lambda function that takes in a raw pointer to the
underlying object of the shared pointer.

Custom deleters are useful in situations such as
```C++
std::shared_ptr<Object> sptr = std::shared_ptr<Object>(
    new Object[3], 
    [](Object* obj) {
        std::cout << "custom deleter\n";
        delete[] obj;
    });
```

where the shared pointer is pointing to an array instead of a single object.

The `get` method in a shared pointer returns the raw object that the shared
pointer is pointing to.

## `enable_shared_from_this`
You may try to implement a method that returns a shared pointer to the current
class
```C++
std::shared_ptr<Object> get_object() {
    return this
}
```

Each call to this function creates a new control block. The first shared
pointer returned from this function to go out of scope will destruct the object
leaving the other pointers dangling. The standard gets around this with
`std::enable_shared_from_this`
```C++
class Object : public std::enable_shared_from_this<Object> {
    std::shared_ptr<Object> get_object() {
        return shared_from_this();
    }
};
```

The caveat to this is that a `std::shared_ptr` of `Object` must be created
before any call to `shared_from_this`. This can often times be done through
making the constructor private and using a static function to create objects.
Call `shared_from_this` before a shared pointer is created throws a
`std::bad_weak_ptr` in C++17. This is because `shared_from_this` looks for an
existing control block.
```C++
class Object : public std::enable_shared_from_this<Object> {
public:
    template <typename... T>
    static std::shared_ptr<Object> Create(T&&... t) {
        return std::make_shared<Object>(std::forward<T>(t)...);
    }
private:
    Object(std::string name);
};
```

The problem with the above code snippet is that `std::make_shared` can't access
the private constructor of `Object`. To get around this, we can create `EnableMakeShared`
that inherits from `Object` and constructs it instead.
`std::shared_ptr<EnableMakeShared>` implicitly convers to a shared pointer to
`Object` preserving the same control block. This works because the derived
class can call the private constructor of a base class. `EnableMakeShared` does
not add any data members and typically will be the same size as `Object`.
```C++
class Object : public std::enable_shared_from_this<Object> {
public:
    template <typename... T>
    static std::shared_ptr<Object> Create(T&&... t) {
        struct EnableMakeShared : public Object {
            EnableMakeShared(T&&... arg) : Object(std::forward<T>(arg)...) {}
        };
        return std::make_shared<EnableMakeShared>(std::forward<T>(t)...);
    }

    std::shared_ptr<Object> get_object() {
        return shared_from_this();
    }
private:
    Object(std::string name);
};
```

`std::enable_shared_from_this` works by stroing a `weak_ptr` in the object
`enable_shared_from_this` object. `std::make_shared` first constructs the
`Object`, at which point the weak pointer is still not set, and after the
object has been cosntructed, sets the weak pointer of the object. It may look
something like:
```C++
// 1. Construct the complete object.
//    - enable_shared_from_this constructor runs (weak pointer empty)
//    - Object constructor runs
//    - EnableMakeShared constructor runs
auto owner = allocate_and_construct<EnableMakeShared>(...);

// 2. All constructors have finished.
//    Library code now sets the inherited weak pointer.
if constexpr (// has enable_shared_from_this<T>) {
    owner->weak_this = owner;
}

// 3. Return the shared pointer.
return owner;
```
