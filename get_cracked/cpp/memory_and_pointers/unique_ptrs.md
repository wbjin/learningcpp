# Unique pointers

Unique pointers are partially specialized so it is able to use arrays as part
of its template argument.
```C++
std::unique_ptr<Object[]> sptr = std::unique_ptr<Object>(new Object[3], });
```

With shared pointers, we had to use a custom deleter to use the `delete[]`
operator instead of `delete`. With unique pointers, we don't have to.

It is still possible for memory leaks to happen with smart pointers. Consider
the following: 

```C++
void func(std::unique_ptr<Obj> obj, int val);

int func2();

func(std::unique_ptr<Obj>(new Obj), func2());
```

The order of evaluation of function parameters is up to the compiler. In this
case, there are three things that must happen before the call to `func`:
- `new Obj` create an object on the heap
- The constructor for `std::unique_ptr` is called
- `func2` is evaluated

The first two and the third can happen in any order. This means that if the
ordering is
1. Object creation
2. Function evaluation, potential exception
3. Unique pointer creation

There can be a potential memory leak. This problem can be avoided by using the
`std::make_unique` and `std::make_shared` functions. The make functions are
also optimized than the constructors of the smart pointers. The regular
constructor of smart pointers allocates the actual object on the heap and the
control block that tracks the metadata for the smart pointer on the heap as
well, thus there are two heap allocations. `std::make_*` only makes one heap
allocation to hold both the control block and the object.

The above reasons are why the make functions are preferred over the
constructors. However, the constructors are needed when passing in custom
deleters.
