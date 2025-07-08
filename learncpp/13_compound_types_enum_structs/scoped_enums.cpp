/*
 * LearnC++ 13.6 Scoped enumerations (enum classes)
 *
 * copmile with make scoped_enums
 *
 * */

#include <iostream>

/*
 * Unscoped enums aren't too safe. For example, if you compare two distinct
 * unscoped enums, because even if a == operator overload isn't implemeneted,
 * the enums will be compared because it will compare the underlying integral
 * type of the unscoped enum.
 * */

/*
 * Scoped enum
 *
 * Scoped enums work the same as unscoped enums except it won't implicitly
 * convert to integers and the enumerators are only scoped within the
 * enumeration (not global scope but only within the enum struct itself).
 *
 * You can't compare scoped enumertaors in different enumerations but you can
 * compare enumerators in the same enumerations.
 *
 * You can convert a scoped enumerator to an integral with static_cast or in
 * C++23, std::to_underlying(). You can also static_cast an integral to an enum
 * class.
 *
 * A common operator overload on scoped enumerations is the unary + operator to
 * convert it to an integral.
 * */

enum class Color {
    red,    // 0
    blue,   // 1
    green,  // 2
};

int operator+(Color c) {
    return static_cast<int>(c);
}



using std::cout, std::endl;
int main() {
    Color color{Color::red}; // red is not directly accessible, it is inside the
                           // scope of the enum class

    cout << +Color::red << "\n";
}

