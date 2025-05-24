This doesn't work
main.cpp
```
#include <iostream>

template <typename T>
T addOne(T x); // function template forward declaration

int main()
{
    std::cout << addOne(1) << '\n';
    std::cout << addOne(2.3) << '\n';

    return 0;
}
```
add.cpp
```
template <typename T>
T addOne(T x) // function template definition
{
    return x + 1;
}
```

Compiler can't see definition for function template addOne in the file so it can't instantiate it. 

The easiest way to fix this is to put all tempalte code into on header file

max.h
```
#ifndef MAX_H
#define MAX_H

template <typename T>
T max(T x, T y)
{
    return (x < y) ? y : x;
}

#endif
```

main.cpp
```
#include "max.h" // import template definition for max<T>(T, T)
#include <iostream>

void foo(); // forward declaration for function foo

int main()
{
    std::cout << max(3, 5) << '\n';
    foo();

    return 0;
}
```