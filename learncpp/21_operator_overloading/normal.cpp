/*
 * LearnC++ 21.3 Overloading operators using normal functions
 *
 * copmile with make nromal
 *
 * */
#include <iostream>

using std::cout;

/*
 * Overloading operators
 *
 * Three ways to overload operators: member function, friend function, and
 * normal function way.
 * */

/*
 * Normal function
 *
 * If you don't need access to member functions of the class, you can overload
 * operators with a numral function.
 * */

class Cents {
public:
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }

private:
	int m_cents {};
};

// Still works without friend declaration in Cents
Cents operator+(const Cents& c1, const Cents& c2) { return Cents {c1.get_cents() + c2.get_cents()}; }
Cents operator+(const Cents& c1, int c2) { return Cents {c1.get_cents() + c2}; }
Cents operator+(int c1, const Cents& c2) { return Cents {c2.get_cents() + c1}; } // reuse the existing overload

int main() {
    Cents a {1};
    Cents b {2};
    Cents c {a + b};

    cout << c.get_cents() << "\n";

    Cents d {a + 2};
    Cents e {1 + b};
}
