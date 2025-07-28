/*
 * LearnC++ 21.5 Overloading operators using member functions
 *
 * copmile with make member
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
 * Member functions
 *
 * For member operator overloaded functions, the left operand is the this object
 * and all other operands are function parameters.
 *
 * Not all operators can be overloaded as friend functions such as =, [], and
 * (). These must be overloaded as member functions
 *
 * Not everything can be overloaded as member functions such as << and >>
 * */

/*
 * When to use each overload method
 *
 * No modification to the left operand: normal or friend
 * Modifications to the left operand: member
 * Unary operators: member
 * Overloading =, [], (), ->: member
 * */

class Cents {
public:
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }

    Cents operator+(const Cents& c2) const;
    Cents operator+(int c2) const;

private:
	int m_cents {};
};

// Still works without friend declaration in Cents
Cents Cents::operator+(const Cents& c2) const { return Cents {m_cents + c2.get_cents()}; }
Cents Cents::operator+(int c2) const { return Cents {m_cents + c2}; }

int main() {
    Cents a {1};
    Cents b {2};
    Cents c {a + b};

    cout << c.get_cents() << "\n";

    Cents d {a + 2};
}
