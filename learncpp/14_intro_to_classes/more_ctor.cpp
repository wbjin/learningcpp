/*
 * LearnC++ 14.13 Temporary class objects
 *
 * copmile with make more_ctor
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Temporary class objects
 *
 * An unnamed object that only exists for the duration of a single expression.
 *
 * Sometimes, you don't need to define a variable to store an object that you
 * only have to use once in a function or scope. You can use a temp object.
 *
 * You can create temporary object with initializer list or letting the compiler
 * try to implicitly match by parameter or return type. For temp objects, direct
 * list initialization and initilization with normal ctor is the same {} vs ()
 * */

/*
 * Copy constructor
 *
 * Constructor that uses an existing object of same type for initialization. If
 * not specified, compiler creates implicity copy ctor for memberwise
 * initialization (match each member and initialize).
 *
 * You can define a copy ctor with a ctor that takes in a const reference. You
 * can access private variables because scoping is by class not by object. The
 * implicit copy ctor almost always preferred
 *
 * Passing and returning an object by value means copy constructor is invoked.
 *
 * Explicitly generate default copy ctor using = default
 *
 * If you do not want copy ctor, use = delete
 * */

/*
 * Rule of Three
 *
 * If you need any of user defined copy ctor, destructor, or copy assignment
 * operator, you probably need all three. Rule of five includes move constructor
 * and move assignment operator
 * */

/*
 * Copy elision
 *
 * Compiler optimization technique that allows compiler to remove unecessary
 * copying of objects. Exempt fro as-if rule, meaning it can copy ctor can be
 * removed even if there is a side effect. This is why we don't want side
 * effects other tahn copying because it can get elided.
 * */

class Temp {
private:
    int _x {};
    int _y {};

public:
    Temp(int x, int y) :
        _x(x),
        _y(y) {}

    Temp(const Temp& temp) = default;

    friend std::ostream& operator<<(std::ostream& out, const Temp& e) {
        out << "Default{_x:" << e._x << ",_y:" << e._y << "}";
        return out;
    }
};

class NoCopy {
public:
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
};

void func(Temp p) {
    cout << p << "\n";
}

int main() {
    func(Temp(1, 2));
    func(Temp{1, 3});
    func({2, 1});

    NoCopy a {};
    // NoCopy b {a}; // Compiler error
}
