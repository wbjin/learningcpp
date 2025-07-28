/*
 * LearnC++ 21.12 Overloading the assignment operator
 *
 * copmile with make assignment
 *
 * */

#include <iostream>

using std::cout;

/*
 * Copy assignment
 *
 * Copy values from one object to another already existing object. Copy
 * constructor initializes a new objet, copy assignment assigns it to an
 * existing object.
 *
 * Copy assignment overload must be a member function.
 *
 * Implicit copy assignment operator
 * Compiler provides implict public copy assignment operator. You can prevent
 * default copy assignment with delete
 * */

/*
 * Shallow vs Deep copying
 *
 * Default copy and assignment operators are shallow copies. It simply does
 * members are copied using the assignment operator. This works well if the
 * class doesn't contain dynamic resources. With dynamic resources, you can end
 * up in a situation where two things point to the same resource.
 *
 * Deep copying is copying by not getting another reference to the underlying
 * resource but by copying the actual contents of the resource.
 * */

class Cents {
public:
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }

    Cents(const Cents& c) = delete; // remove implicit copy assignment operator

    Cents& operator=(const Cents& c) {
        if (&c == this) return *this; // do nothing for self assignment
        m_cents = c.m_cents;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream&, const Cents&);

private:
	int m_cents {};
};

std::ostream& operator<<(std::ostream& o, const Cents& c) {
    o << "Cents{" << c.get_cents() << "}";
    return o;
}

int main() {
    Cents a {5};
    Cents b {1};

    cout << b << "\n";
    b = a;
    cout << b << "\n";
}
