/*
 * LearnC++ 17.5 Arrays of references via std::reference_wrapper
 *
 * copmile with make reference_array
 *
 * */

#include <array>
#include <functional>
#include <iostream>

using std::cout, std::endl;

/*
 * Reference Arrays
 *
 * Arrays can hold any type. This means you can put pointers in arrays. However,
 * references are not objects so you can't make an array of references. You can
 * use std::reference_wrapper as a workaround.
 *
 * std::reference_wrapper
 *
 * Behaves like a modifiable lvalue reference to a type template arument T.
 * Using an = with std::reference_wrapper will reseat the reference (change what
 * it is refering to). It implicitly converts to the underlying T&, and the
 * get() function is used to get a T& which is useful when we want to update the
 * value of the object being refered.
 *
 * std::ref() and std::cref() are shortcuts to create a std::reference_wrapper
 * and a const std::reference_wrapper.
 * */

int main() {
    int a {1};
    int b {2};
    // std::array<int&, 2> c {a, b}; // error

    std::array<std::reference_wrapper<int>, 2> d {a, b};
    d[0].get() = 2;

    cout << a << "\n";

    auto e {std::ref(a)};
    e.get() = 3;
    cout << a << "\n";

    const auto f {std::cref(a)};
    // f.get() = 4; // error because it is a const reference
}
