/*
 * LearnC++ 7.9 Inline function and variables
 *
 * copmile with make inline
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Inline expansion: Function is repalced with the code from the definition
 *
 * Doesn't make everything faster, makes program executable larger which tends to slower performance because it doesn't fit in instruction cache as well
 *
 * inline keyword suggests to compiler to expand the function whre it is being called
 *
 * Modern compilers can determine which function calls to make inline, rely on this and don't add it yourself
 *
 * Inline meaning has changed to mean multiple definitions are allowed. inline function is allowed to be defined in multiple translation unites
 * Inline functions need to satisfy these requirements
 * Compiler needs ot be able to see full definition in each translation unit
 * Definition can occure after point of use if forward declaration is provided
 * Every definition for an inline funciton with external linkage must be identical, undefined behavior otherwise
 *
 * Inline functions are typically defined in header files so the definitions are the same everywhere it is used
 *
 * These functions are implicitly inline
 * Functions in classes, structs, or union type definitions
 * Constexpr / consteval functions
 * Functions implicitly instantiated from functino templates
 * */

inline int min(int x, int y) {
    return (x < y) ? x : y;
}

// Both inline.cpp and helper.cpp has the same function with identifier
inline double pi() { return 3.14159; }

int main() {
    cout << min(3, 4) << endl;
}
