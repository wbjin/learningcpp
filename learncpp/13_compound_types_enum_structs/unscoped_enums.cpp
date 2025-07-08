/*
 * LearnC++ 13.2 Unscoped enumerations
 *
 * copmile with make unscoped_enums
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Unscoped enumerations
 *
 * Defined via enum keyword and allows you to scope the nameable keywords for a
 * type. Enumerated types are distinct so different enums are distinct types.
 * Unscoped enums (just enum keyword) has a global scope, this means the
 * namespace is global and it can conflict with others. Enumeration refer to the
 * type (Color) and enumerator refer to the value in enumeration (red, green,
 * blue). Each enumerator is assigned an integer value based on the position in
 * the enumeration list. By default, the first enumerator has a value of 0. You
 * can assign custom enumeration integral values where non defined enumerators
 * are given a value one greater than the previous enumerator. Duplicate
 * integral values can be assigned to the different enumerators. In that case,
 * the enumerators are interchangeable. When initializing an enumeration, it
 * will be zero-initialized with the the integral value 0 even if there is no
 * corresponding enumrator with the value. Put the invalid or unknown
 * unumeration case with value 0
 *
 * Unscoped enums will implicitly conver to integers if there is no defined
 * conversion for the function.
 *
 * The underlying type of unscoped enums is implementation defined. You can
 * define it when defining one though but the underlying type must be an
 * integral type. This is why unscoped enums will implicitly conver to ints.
 * However, integral types won't conver to unscoped enums implicitly. You can do
 * it explicitly with static_cast. Casting to a value out of the range of the
 * enum will be undefined behavior.
 * */

/*
 * Overloading unscoped enum
 *
 * When you overload an operator, compiler will look for an overloaded
 * operator<< that can ahndle std::ostream and type that is being put into <<.
 * Then << returns the left operand which is the std::ostream
 * */

enum Color {
    red,    // 0
    green,  // 1
    blue,   // 2
};

enum Animal : int16_t {
    cat = -3,
    dog,        // -2
    pig,        // -1
    horse = 5,
    giraffe = 5,
    chicken     // 6
};

std::ostream& operator<<(std::ostream& out, const Color& color) {
    switch (color) {
    case red:
        out << "red";
        break;
    case green:
        out << "green";
        break;
    case blue:
        out << "blue";
        break;
    }
    return out;
}

int main() {
    cout << dog << "\n"; // prints -2
    cout << sizeof(dog) << "\n"; // 2 bytes
    Animal a {static_cast<Animal>(cat)};

    cout << red << "\n"; // overloaded << operator for Color
}

