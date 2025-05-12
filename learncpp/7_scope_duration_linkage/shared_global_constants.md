Prior to C++17
1. Create header file
2. Create namespace in header file and add constants in namespace
3. INclude header file

Example
constants.hpp
```
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define your own namespace to hold constants
namespace constants
{
    // Global constants have internal linkage by default
    constexpr double pi { 3.14159 };
    constexpr double avogadro { 6.0221413e23 };
    constexpr double myGravity { 9.2 }; // m/s^2 -- gravity is light on this planet
    // ... other related constants
}
#endif
```

main.cpp
```
#include "constants.h" // include a copy of each constant in this file

#include <iostream>

int main()
{
    std::cout << "Enter a radius: ";
    double radius{};
    std::cin >> radius;

    std::cout << "The circumference is: " << 2 * radius * constants::pi << '\n';

    return 0;
}
```

Const globals have internal linkage, every time you include the header file you copy the variable. Changing constant value means you have to recompile every, large constants will make things inefficient.

Can instead put the constexpr as extern const and put the definition in a cpp file. This way, it is initialized once in the cpp file and used elsewhere. Downside is that outside of the file constants are defined in, variables can't be used for constant expressions.

Global constants as inline variables (C++17)
Make the constants inline. You can include header multiple times without violating ODR

constants.h
```
#ifndef CONSTANTS_H
#define CONSTANTS_H

// define your own namespace to hold constants
namespace constants
{
    inline constexpr double pi { 3.14159 }; // note: now inline constexpr
    inline constexpr double avogadro { 6.0221413e23 };
    inline constexpr double myGravity { 9.2 }; // m/s^2 -- gravity is light on this planet
    // ... other related constants
}
#endif
```

main.cpp
```
#include "constants.h"

#include <iostream>

int main()
{
    std::cout << "Enter a radius: ";
    double radius{};
    std::cin >> radius;

    std::cout << "The circumference is: " << 2 * radius * constants::pi << '\n';

    return 0;
}
```

Variables are only instantiated once and shared across the code files.