/*
 * LearnC++ 6.7 Relational operators and floating point comparisons
 *
 * copmile with make float_comparison
 *
 * */
#include <iostream>
#include <cmath>

using std::cout, std::endl;

/*
 * Floats are unsafe to compare with == and != because floats have rounding error. For example, a value may have 1.0 as 1.000000001 depending on the precision.
 * */

// Algorithm from Donald Knuth
// Epsilon (the difference to allow) is relatie to magnitude of bigger of a and b
bool approximatelyEqualRel(double a, double b, double relEpsilon)
{
	return (std::abs(a - b) <= (std::max(std::abs(a), std::abs(b)) * relEpsilon));
}

// Combination of algorithm from Donald Knuth and absolute epsilon
// In C++23, can be made constexpr because std::abs became a constexpr
bool approximatelyEqualAbsRel(double a, double b, double absEpsilon, double relEpsilon)
{
    // Check if the numbers are really close -- needed when comparing numbers near zero.
    if (std::abs(a - b) <= absEpsilon)
        return true;

    // Otherwise fall back to Knuth's algorithm
    return approximatelyEqualRel(a, b, relEpsilon);
}

int main() {
    constexpr double relEps { 1e-8 };
    constexpr double absEps { 1e-12 };

    // Expect to be 1.0 but there are some rounding issues
    constexpr double a { 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 };
    cout << a << "\n";

    cout << std::boolalpha;
    cout << approximatelyEqualAbsRel(a, 1.0, absEps, relEps) << "\n"; // Comparing almost 1.0 to 1.0
    cout << approximatelyEqualAbsRel(a-1.0, 0.0, absEps, relEps) << "\n"; // Comparing almost 0.0 to 0.0
}
