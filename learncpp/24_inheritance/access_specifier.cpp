/*
 * LearnC++ 24.5 Inheritance and access specifiers
 *
 * copmile with make access_specifiers
 *
 * */

#include <iostream>

using std::cout;

/*
 * Access specifiers
 *
 * Public: Can be accessed by anybody
 * Private: Can be accessed only by member of functions of same class or friends
 * Protected: Can be accessed by class, friends, and derived classes
 *
 * You can also inherit from a base class as public, private, or protected. This
 * means 9 combinations. In a nutshell, the access specifier in the base class
 * may be changed depending on the inheritance type.
 * */

/*
 * Public inheritance
 *
 * public access specifier => public
 * protected access specifier => protected
 * private access specifier => can't access
 * */

/*
 * Protected inheritance
 * 
 * public access specifier => protected
 * protected access specifier => protected
 * private access specifier => can't access
 * */

/*
 * Private inheritance
 * 
 * public access specifier => private
 * protected access specifier => private
 * private access specifier => (can't access)
 * */

class Base {
public:
    int _public {};
protected:
    int _protected {};
private:
    int _private {};
};

class Pub : public Base {
public:
    Pub() {
        _public = 1;
        _protected = 1;
        // _private = 1; // error
    }
};

class Priv : private Base {
public:
    Priv() {
        _public = 1;
        _protected = 1;
        // _private = 1; //error
    }
};

int main() { 
    Pub pub {};
    cout << pub._public << "\n";
    // cout << pub._protected << "\n"; // error
    // cout << pub._private << "\n": // error

    Priv priv {};
    // cout << priv._public << "\n"; // error
}
