/*
 * LearnC++ 21.2 Overloading the arithmetic operators using friend functions
 *
 * copmile with make friend
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
 * Friend function way
 *
 * Declare a friend function with operator+ overload. Takes in two cents objects
 * and returns a new Cents object.
 *
 * Overloading operator with two of the same types works either way ie a + b
 * uses the same function as b + a. However, order matters with overload with
 * different types a + b is not the same as b + a. You end up needing to write
 * two versions.
 * */

class Cents {
public:
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }

    friend Cents operator+(const Cents& c1, const Cents& c2);
    friend Cents operator+(const Cents& c1, int c2);
    friend Cents operator+(int c1, const Cents& c2);

private:
	int m_cents {};
};

Cents operator+(const Cents& c1, const Cents& c2) { return Cents {c1.m_cents + c2.m_cents}; }
Cents operator+(const Cents& c1, int c2) { return Cents {c1.m_cents + c2}; }
Cents operator+(int c1, const Cents& c2) { return Cents {c2.m_cents + c1}; } // reuse the existing overload

int main() {
    Cents a {1};
    Cents b {2};
    Cents c {a + b};

    cout << c.get_cents() << "\n";

    Cents d {a + 2};
    Cents e {1 + b};
}
