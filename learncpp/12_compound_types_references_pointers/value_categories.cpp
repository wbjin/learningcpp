/*
 * LearnC++ 12.2 Value Cateogries (lvalues and rvalues)
 *
 * copmile with make value_categories
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * All expressions in C++ have a type and a value category
 * Value category indicate whether expression resolves to a value, function, or an object
 * Before C++11, there were lvalue and rvalue
 * Since C++11, there are glvalue, prvalue, xvalue as well
 *
 * lvalue: Expression that evaluates to an identificable object or function
 * "identity" means an entity (object or funtion) that can be differenctiated from other entities. Entities are accessible with identiifer, reference, or pointer
 * lvalues are modifiable or non-modifiable (const)
 *
 * rvalue: Expression that is not a lvalue. Expression that evaluate to a value like literals (except for C-style strings) and return value of functions and operators
 * Rvalues aren't identifiable, the lifetime is only within the scope of the exprsesion where it is used
 *
 * Lvalue expression -> Evaluate to an object that can be named
 * Rvalue expression -> Evalutate to an value
 *
 * Assignment operations expect a rvalue on the right side. Assigning an lvalue exprsesion to another lvalue expression causes lvalue-to-rvalue conversion
 * lvalues can implicitly convert into rvalues, lvalues can be used anywhere rvalues are expected
 * rvalues cannot implicit convert to a lvalue
 * */

int func() {
    return 1;
}

// Code to check if something is lvalue or rvalue
// T& is an lvalue reference, so this overload will be preferred for lvalues
template <typename T>
constexpr bool is_lvalue(T&)
{
    return true;
}

// T&& is an rvalue reference, so this overload will be preferred for rvalues
template <typename T>
constexpr bool is_lvalue(T&&)
{
    return false;
}

// A helper macro (#expr prints whatever is passed in for expr as text)
#define PRINTVCAT(expr) { std::cout << #expr << " is an " << (is_lvalue(expr) ? "lvalue\n" : "rvalue\n"); }

int main() {
    int a {1}; // 1 is an rvalue expression
    const double b {1.1}; // 1.1 is an rvalue expression
    int c {a}; // a is modifiable l value
    const double d {b}; // b is non-modifiable l value
    int e {func()}; // func() is an rvalue expression
    int g {a + 1};  // a+1 is a rvalue expression

    int h = a; // a undergoes lvalue to rvalue conversion
    
    PRINTVCAT(a);
    PRINTVCAT(1);
    PRINTVCAT(func());
    PRINTVCAT(++a);
    PRINTVCAT(a++);
}

