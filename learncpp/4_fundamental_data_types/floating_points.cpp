/*
 * LearnC++ 4.7 Scientific notation
 *
 * copmile with make floating_points
 *
 * */
#include <iostream>
#include <limits>
#include <iomanip>

using std::cout, std::endl;

/*
 * Floats are implemented in the IEEE-754 standard. Most of the times floats are 4 bytes, doubles are 8 bytes
 * However, long double is less strictly defined. It can be 8 to 16 bytes and it may not even use IEEE 754 compliant format
 * */

int main() {
    // Check whether the different float types are compliant
    std::cout << std::numeric_limits<float>::is_iec559 << "\n";
    std::cout << std::numeric_limits<double>::is_iec559 << "\n";
    std::cout << std::numeric_limits<long double>::is_iec559 << "\n";

    // Floats have precision between 6 to 9 digits
    [[maybe_unused]] float f{1.0f}; // f tells compiler 1.0 is a floating point literal

    // Doubles have precision between 15 to 18 digits
    [[maybe_unused]] double d{1.0}; // decimals are doubles by default
    
    // Long doubles have minimum precision of 15, 18, or 33
    [[maybe_unused]]long double ld{1.0};

    // std::cout has some default precision, override with std::setprecision()
    std::cout << std::setprecision(17) << std::endl;
    std::cout << 3.33333333333333333333333333333333333333f <<'\n'; // f suffix means float
    std::cout << 3.33333333333333333333333333333333333333 << '\n'; // no suffix means double

    // Prefer doubles over floats if space is not a bottleneck. Doubles have higher precision

    // Floating point comparisions are tricky because floating points have limited precision
    double d2{ 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 + 0.1 };
    if (d2 == 1.0) 
        cout << "Sum to 1.0\n";
    else
        cout << "Sum to " << d2 << "\n";
}
