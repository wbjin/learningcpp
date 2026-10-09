# Raw Pointers

In C++, top level constness is removed from function signatures while low level
constness is preserved. Consider the function signatures below
```C++
void g(double*);
void g(const double*);

void h(double*);
void h(double* const);
```

Function `g` has two overloads, a pointer to double and a pointer to a const
double. The pointer to a const double is low level const and is preserved by
the function signature.

Function `h` appears like it has two overloads, a pointer to double and a const
pointer to a double. This is high level const and is not preserved. The second
`h` actually becomes a redeclaration instead of an overload.

A top level const makes the variable itself unchangeable. Low level const makes
the underlying variable of the reference unchangeable. C++ ignores top level
const on parameters because a param passed by value is the function's own copy.

The following code snippet
```C++
std::cout << std::is_same_v<void(double*), void(const double*)>;
```
will print 0 because low level const is prserved.
