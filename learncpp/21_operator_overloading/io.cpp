/*
 * LearnC++ 21.4 Overloading I/O operators
 *
 * copmile with make io
 *
 * */
#include <iostream>

using std::cout, std::cin;

/*
 * Overloading operators
 *
 * Three ways to overload operators: member function, friend function, and
 * normal function way.
 * */

/*
 * IO operators <<, >>
 *
 * You can overload the << and >> operators for cout, cin, etc
 * All you need to do is make an operator that takes in std::ostream or
 * std::istream and an object that you are trying to overload the print for and
 * return the ostream or istream object by reference.
 *
 * If you want to chain binary operators together, you hvae to return the left
 * operand by reference.
 *
 * For overloading istream, you should avoid partial extraction. You should only
 * change the passed in object if you are able to extract all values needed to
 * initialize the object.
 *
 * If the extracted value doesn't make semantically, put the input stream in
 * failure mode with std::cin.setstate(std::ios_base::failbit)
 * */

class Cents {
public:
    Cents() = default;
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }
     
    friend std::ostream& operator<<(std::ostream&, const Cents&);
    friend std::istream& operator>>(std::istream&, Cents&);

private:
	int m_cents {};
};

// Still works without friend declaration in Cents
Cents operator+(const Cents& c1, const Cents& c2) { return Cents {c1.get_cents() + c2.get_cents()}; }
Cents operator+(const Cents& c1, int c2) { return Cents {c1.get_cents() + c2}; }
Cents operator+(int c1, const Cents& c2) { return Cents {c2.get_cents() + c1}; } // reuse the existing overload

std::ostream& operator<<(std::ostream& o, const Cents& c) {
    o << "Cents{" << c.get_cents() << "}";
    return o;
}

std::istream& operator>>(std::istream& in, Cents& c) {
    int cents;
    in >> cents;
    if (cents < 0)
        in.setstate(std::ios_base::failbit);
    else
        c.m_cents = cents;
    return in;
}

int main() {
    Cents a {1};
    Cents b {2};
    Cents c {a + b};

    cout << c.get_cents() << "\n";

    Cents d {a + 2};
    Cents e {1 + b};

    cout << e << "\n";

    Cents f {};
    cin >> f;
    cout << f << "\n";
}
