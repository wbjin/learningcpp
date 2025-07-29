/*
 * LearnC++ 22.2 R-value references
 *
 * copmile with make rvalue_ref
 *
 * */

#include <iostream>

using std::cout;

/*
 * Lvalue references
 *
 * Lvalue references are initialized with a single & ie int& y {x}
 *
 * A normal lvalue reference can only be initialzied with a modifiable lvalue.
 * Lvalue references to const can be initialized with modifiable lvalues,
 * non-modifiable lvalues, and rvalues (rvalue because of temporary variable
 * being extended to match the reference lifetime). Lvalue references are
 * usefuly because you can pass in rvalues (temp objects) and still have a valid
 * reference.
 * */

/*
 * Rvalue references
 *
 * Reference that is designed to be initialized with an rvalue only. Rvalue
 * references are initialized with two & ie int&& x {1}
 *
 * Rvalue references cannot be initialized with any lvalue. It is only
 * initialized with rvalues.
 *
 * The lifetime of the object used to initialize rvalue reference extended to
 * match lifespan of object rvalue reference.
 *
 * Rvalue references are used mostly for function parameters so you can override
 * based on lvalue and rvalue arguments.
 *
 * Note that rvalue references are lvalues.
 * */

void value(int&) {
    cout << "lvalue\n";
}

void value(int&&) {
    cout << "rvalue\n";
}

int main() {
    int a {1};
    int& b {a}; // lvalue reference

    const int c {2};
    // int& d {d}; // error, lvalue references can't be initialiezd with
    // nonmodifiable lvalue
    // int& e {3}; // error, lvalue references can't be initialized with rvalue

    const int& f {a};
    const int& g {c};
    const int& h {2}; // lvalue references to const can be initialized with rvalues

    int&& i {2}; // rvalue reference, i is actually a lvalue
    // int&& j {a}; //error, rvalue references can't be initialized with lvalue
    // int&& k {c}; //error, rvalue references can't be initialized with const lvalue

    const int&& l {3};
    // const rvalue references same init rules as non const rvalue reference

    value(a);
    value(5);
    value(b);
    value(i);
}
