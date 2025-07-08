/*
 * LearnC++ 13.7 Introduction to structs
 *
 * copmile with make struct
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Initializer list
 *
 * Member variables will be initialized in order of initalizer list. If there
 * are less initialization values than the number of members, members withd
 * default values will be initialized with default value. Otherwise, member is
 * copy-initialized from empty initializer list. This means value initialization
 * with the values passed in, if all values in initializer list is used up,
 * value initialized
 *
 * Structs can also be initialized with copy, direct, and direct-list
 *
 * Best practice: Structs should be owners of data not viewers. This means
 * trying not to put pointers or references as member variables
 * */

struct Example {
    int id {};
    int age {};
    double wage {6000.0};
    int x {};
};

std::ostream& operator<<(std::ostream& out, const Example& e) {
    out << "Example{id:" << e.id << ",age:" << e.age << ",wage:" << e.wage << "}";
    return out;
}

int main() {
    Example a {1, 24}; // x will be value initialized to 0.0
    cout << a << "\n";

    Example b{.id{2}, .age{25}, .x{2}}; // C++ 220 designated initializer. Order needs
                                 // to match order of member variables
    cout << b << "\n";

    Example c = a;
    cout << c << "\n";
    Example d(a);
    cout << d << "\n";
    Example e {a};
    cout << e << "\n";

    Example f; // No initialization at all
    cout << f << "\n";
    Example g; // Value initialize everything without default initializer
    cout << g << "\n";
}

