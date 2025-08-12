/*
 * LearnC++ 24.7 Calling inherited functions and overriding behavior
 *
 * copmile with make overriding
 *
 * */

#include <iostream>

using std::cout;

/*
 * Inheriting functions
 *
 * Derived classes inherit all behaviors of base class. When a function is
 * called on derived object, compiler checks derviced class for matches with
 * argument types. If no match exists, compiler walks up the inheritance chain.
 *
 * When overriding a function in the derived class, the access specifier of the
 * derived class is used, not the base classes ie, private function in base can
 * be overriden as public in derived.
 *
 * You can use the use keyword which tells compiler that all functions with a
 * certain name is visible in a derived class.
 *
 * This means you can hide functions (make public in base private in derived) or
 * show functions (vice versa)
 *
 * You can also delete an inherited function with = delete;
 * */

class Base {
public:
    Base() { }

    void identify() const { std::cout << "Base::identify()\n"; }

    void print() const { std::cout << "Base{}\n"; }
};

class Derived: public Base {
public:
    Derived() { }
};

class Derived2: public Base {
public:
    Derived2() { }

    using Base::print;

    void print(int i) const { std::cout << "Derived{" << i << "}\n"; }

    void identify() const { std::cout << "Derived2::identify()\n"; }
};

int main() { 
    Derived a {};
    a.identify();

    Derived2 b {};
    b.identify();
    b.print();
    b.print(1);
}
