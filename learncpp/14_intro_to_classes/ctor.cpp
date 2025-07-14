/*
 * LearnC++ 14.10 Constructor member initializer lists
 *
 * copmile with make ctor
 *
 * */

#include <iostream>
#include <utility>

using std::cout, std::endl;

/*
 * Initializer list
 *
 * Constructor member initialization is done in order that members are defined
 * in the class even if you do it in a different order in the initialization
 * list. Rule of thumb, follow the order of declaration in class for initializer
 * list.
 *
 * Priority of initialization
 * 1) Listed in member Initializer list
 * 2) Has default member initializer
 * 3) Default initialized
 *
 * Member variables that are const or references need to be initialized in
 * initializer list not constructor body. Initialization is considered done
 * after initializer list, cosntruction is done after constructor body.
 *
 * When construction fails (violate some property), it's best to throw an
 * exception because you can't change return value of constructor. Otherwise,
 * it's best to have a init function that returns a std::optional of the class.
 * */

/*
 * Default constructor
 *
 * Use value initialization for default constructing (no arguments into
 * constructor) a class. Rightmost arguments can also be default arguments. If
 * all arguments are default arguments, constructor is default constructor.
 *
 * Only one default constructor per class.
 *
 * A class with no explicitly defined constructor will have a implicit default
 * constructor genereated by the compiler that does nothing. You can use =
 * default to explicitly tell the compiler to generate a default constructor. If
 * we define any constructor with arguments but also want a default constructor,
 * you can use =default.
 *
 * Empty user defined vs defaulted default ctor
 * When value initializing a class, if there is a user defined empty ctor, the
 * object will be default initialized. If there is a default constructor with =
 * default or implicit, it will be zero initialized then default initiliazed.
 * */

/*
 * Delegating Ctor
 *
 * Rule of thumb, don't call constructor in the body of a member function or
 * constructor. Instead, use the initializer list to delegate construction.
 * Usually the ctor with less arguments delegates to the ctor with more
 * arguments by adding default args. It is possible to have an infiinte loop of
 * delegating ctors.
 * */

int func(int a) {
    cout << a << "\n";
    return a;
}

class Example {
public:
    Example() = default; // explicit default constructor

    Example(int x, int y) :
        _y(func(y)),
        _x(func(x)) {}

    Example(int x, int y, int z) :
        _x(func(x)), 
        _y(func(y)),
        _z(func(z)) {}

    friend std::ostream& operator<<(std::ostream& out, const Example& e) {
        out << "Example{_x:" << e._x << "_y:" << e._y << "_z:" << e._z << "}";
        return out;
    }

private:
    int _x {};
    int _y {};
    int _z {1};
};

class User {
public:
    User() {}

    friend std::ostream& operator<<(std::ostream& out, const User& e) {
        out << "User{_x:" << e._x << "}";
        return out;
    }
private:
    int _x;
};

class Default {
public:
    Default() = default;

    friend std::ostream& operator<<(std::ostream& out, const Default& e) {
        out << "Default{_x:" << e._x << "}";
        return out;
    }

private:
    int _x;
};

class Delegate {
public:
    Delegate(int x) : Delegate(x, 0) {}

    Delegate(int x, int y) :
        _x {x},
        _y {y} {}

    friend std::ostream& operator<<(std::ostream& out, const Delegate& e) {
        out << "Default{_x:" << e._x << ",_y:" << e._y << "}";
        return out;
    }
private:
    int _x {-1};
    int _y {-1};
};

int main() {
    Example a(1, 2); // 1 will print before 2 even though _y comes first in list cout << a << "\n"; // _z will be 1
    Example b(1, 2, 3);
    cout << b << "\n";

    Example c {};
    cout << c << "\n";

    User d {};
    cout << d << "\n"; // might not be 0 because not zero initialized

    Default e {};
    cout << e << "\n"; // should be 0

    Delegate f(1);
    cout << f << "\n"; // shoud be 1, 0

    Delegate g(1, 2);
    cout << g << "\n"; // should be 1, 2
}
