/*
 * LearnC++ 21.11 Overloading typecasts
 *
 * copmile with make typecast
 *
 * */

#include <iostream>

using std::cout;

/*
 * Overloading typecast
 *
 * If you want to convert an object to a different type, you can overload
 * typecasting
 *
 * Overloaded typecast does not have explicity parameters and there is no way to
 * pass explicit arguments except for the *this
 *
 * It will also allow implict casts
 *
 * You can make typecasts explicit so that it can't be used for implicit
 * conversions. It can only be invoked with static_cast
 *
 * Use overloaded typecasts when
 * - Providing conversion to a fundamental type
 * - Conversion returns a reference or const reference
 * - Conversion to a type you can't add members to
 * */

class Cents {
public:
	Cents(int cents) : m_cents{ cents } { }
	int get_cents() const { return m_cents; }
    operator int() const {return m_cents;} // cast a Cents to int

private:
	int m_cents {};
};

int main() {
    Cents a {5};
    int b {int(a)};
    cout << b << "\n";
}
