/*
 * LearnC++ 10.5 Arithmetic conversions
 *
 * copmile with make arithmetic_conversions
 *
 * */
#include <iostream>
#include <typeinfo>

using std::cout, std::endl;

/* 
 * Some operators require operands to have the same type. In this case, one of the operands will be implicitly converted to a matching type called the common type between the operands
 * The following operators require operands to be the same type
 * Binary arithmetic: +, -, *, /, %
 * Binary relational: <, >, <=, >=, ==, !=
 * Binary bitwise: &, ^, |
 * Coinditional: ?
 *
 * Conversion rules. It is complex but it can be simplified to something like this.
 * There is a ranked list of types. (highest) long double, double, float, long long, long, int (lowest)
 * 1. If one is an integral and other is floating point, integral operands converted to type of floating point. Otherwise, integral operands are numerically promoted to match the higher ranked one
 * 2. After promotion, Operand with lower rank converted to operand with higher rank. However, if one is signed and the other is unsigned.
 *  a. If rank of unsigned is greater than rank of signed, signed converted to type of unsigned
 *  b. If type of signed can represent all values of type of unsigned, unsigned converted to signed
 *  c. Otherwise, both converted to corresponding unsigned type of signed operand
 * */

int main() {
    int a{1}; // i
    double b{1.0}; // d
    cout << typeid(a + b).name() << endl; // d
    
    short c{1};
    short d{2};
    cout << typeid(c + d).name() << endl; // i because short is not in priority list so both promoted to type int
    
    cout << typeid(5u - 10).name() << endl; // u because int promoted to unsigned int

    cout << std::boolalpha << (-3 < 5u) << endl; // false because -3 promoted to unsigned int
}
