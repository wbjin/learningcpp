/*
 * LearnC++ 5.4 The as-if rule and compile-time optimization
 *
 * copmile with make optimization
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * as-if rule:
 * Compiler can modify program however it likes in order to produce more optimized code as long as the modification do not affect a program's observable behavior
 * Exception is unnecessar calls to copy or move constructors that can be omitted. (Copy elision)
 *
 * */

int main() {
    // Compile time optimiations

    // Constant folding
    int a {1 + 2}; // Evaluated at compile time as 3
    cout << a << "\n";

    // Constant propagation
    const int b {3}; // 3 is stored in memory
    cout << b << "\n"; // 3 is accessed from memory. Replaces values with know constant values with their values so you don't have to fetch from memory
    // Help compiler optimize by putting const qualifier and constant variables
    
    // Dead code elimination
    int c {4}; // c is never used so is removed by the optimizer
}
