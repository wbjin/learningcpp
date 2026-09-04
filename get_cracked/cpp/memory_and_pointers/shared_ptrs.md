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
