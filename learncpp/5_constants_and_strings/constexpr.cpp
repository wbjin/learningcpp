/*
 * LearnC++ 5.5 Constant expressions 5.6 Constexpr variables
 *
 * copmile with make constexpr
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Compile-time programming
 * C++ provides a way to explicitly tell compiler what part of the code we want executed at compile time
 * constexpr variables, constexpr functions, templates, static_asserts
 *
 * Constant expression: An expression where each part must be evaluatable at compile-time
 *
 * Constant expression usable
 * Literals, operators with constant expression operands, const integral values with constant initializer, constexpr variables, constexpr function calls
 * Not usable if non-const, const non-integral value, return value of non-constexpr functions, etc
 *
 * Just because something is a constant expression, doesn't mean it is evaluated at compile time
 * Always: Constant expression used in a context that requires a constant expression
 * Likely: Constant expression used in context that does not require a constant expression
 * Possible: Non-constant expression where compiler is able to determine all values at compile-time
 * Never: Non-constant expression where copmiler is not able to deteremine all values at compile-time
 * */

/*
 * Constexpr variable: Always evaluated at compile-time. Must be initialized with cosntant expressions
 *
 * Constexpr is implicitly const but const is not constexpr.
 *
 * Runtime const should be defined as const. Compile time const should be defined as constexpr
 * */

int four() {
    return 4;
}

constexpr int five() {
    return 5;
}

int main() {
    // Compile error
    // constexpr double d;
    constexpr double d {1};
    
    constexpr int e {five()};
    // constexpr int f {four()}; Compile error because four() does not return a constexpr
}
