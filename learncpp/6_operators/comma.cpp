/*
 * LearnC++ 6.5 The comma operator
 *
 * copmile with make comma
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Comma operator: Evaluate multiple expressions wherever a single expression is allowed. Evaluate left operand then the right operand return the result of the right operand
 * */

int main() {
    int x {};
    cout << (++x, ++x) << "\n"; // Returns 2, x incremented to 1 in left, incremented to 2 in right
    
    // Has lowest assignment precedence
    int y = (++x, x);
    // Different
    y = ++x, x;
}
