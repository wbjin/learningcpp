/*
 * LearnC++ 13.8 Class templates
 *
 * copmile with make class_templates
 *
 * */

#include <iostream>
#include <utility>

using std::cout, std::endl;

/*
 * Class templates
 *
 * Template definition of a class.
 *
 * Class templates can have templated and non-templated types as well as
 * multiple templated types.
 *
 * Like function templates, templated classes can be defined in header files.
 * Template definitions are exempt from one definition rule.
 */

/*
 * Class template argument deduction (CTAD)
 *
 * From C++17, when instantiating object from class template, compiler can
 * deduce the template types from the types of the object's initializer list. It
 * only happens when there is no template argument list.
 *
 * Sometimes, CTAD doesn't work right out of the box (fixed in C++20
 * apparently). Need to give compiler hints in the form of deduction guide.
 * Deduction guide tells compuler how initializer arguments should be formatted
 * to template arguments.
 *
 * You can also give default template parameters. It will be used when template
 * parameters aren't explicitly specified and can't be deduced.
 *
 * CTAD doesn't work with non-static member initialization and function
 * parameters. Basically, don't use to initialize member variables and function
 * params.
 * */

/*
 * Alias templates
 *
 * Just like how you can create type aliases like using Num = int;, you can do
 * the same with template aliases.
 *
 * If we want to create type alias for templates where not all of the template
 * arguments are defined (some we want user to define), we can use alias
 * template.
 *
 * Alias templates must be defined in global scope. Prior to C++20, can't use
 * CTAD for alis templates so need to explicitly provide template type.
 * */

template <typename T = int>
struct Pair {
    T first{};
    T second{};
};

// Need one with two arguments because Pair has two members
template <typename T>
Pair(T, T) -> Pair<T>;

template <typename T>
constexpr T max(Pair<T> p) {
    return (p.first < p.second ? p.second : p.first);
}

template <typename T, typename U>
struct Pair2 {
    T first {};
    U second {};
};
template <typename T, typename U>
Pair2(T, U) -> Pair2<T, U>;

template <typename T, typename U>
constexpr T max(Pair2<T, U> p) {
    return (p.first < p.second ? p.second : p.first);
}

// Alias template
template <typename T>
using UserPair = Pair2<uint32_t, T>; // Pair with uint32_t and any other type.


int main() {
    Pair<int> p_int {};
    Pair<double> p_double {};

    Pair2<int, double> p_both {1, 2.0};
    cout << max(p_both) << "\n";

    Pair a{1, 2}; // Deduces Pair<int> but with help
    cout << max(a) << "\n";

    std::pair b {1, 2.0}; // Deduces std::pair<int, double> because deduction guide present

    Pair2 c{2.9, 1};
    cout << max(c) << "\n";

    Pair d; // Pair<int>

    using UPair = Pair<uint32_t>;
    UPair e {2, 3};

    UserPair<double> f {1, 2.3};
    cout << max(f) << "\n";
}

