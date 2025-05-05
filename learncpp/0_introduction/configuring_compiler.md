Build configurations

- Debug builds: `-ggdb`
- Release builds: `-O2 -DNDEBUG`

Compiler configuraiton

Warning flags
- `Wall`: Enables widset of useful ewarnings such as unused and/or uninitialized variables, possible type mismatches, implicit type conversions
- `Weffc++`: Warnings based off of "Effective C++" book. Warnings such as missing virtual destructors in base class, classes with pointer member variables that lack copy constructors and assignment operators
- `Wextra`: Additional warnings beyond those in `Wall`
- `Wconversion`: Enables warnings about implicit type conversions that can alter the value eg, narrowing conversions
- `Wsign-conversion`: Enables warnings about implicit conversions between signed and unsigned types

I also like to use
- `Wpedantic`

Add these warnings in CMake using `target_compile_options` or `add_compile_options`
```
target_compile_options(MyTarget PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Weffc++
)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Weffc++
)
```

You can set C++ standard versions using `target_compile_features` or `set`
```
target_compile_features(MyTarget PRIVATE cxx_std_17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```