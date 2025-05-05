/*
 * LearnC++ 1.4 Variable assignment and initialization
 *
 * copmile with make initialization
 *
 * */
#include <iostream>

using std::cout, std::endl;

int main() {
    // Default initilization
    int a; // Value of a undefined

    // Copy initilization
    int b = 1; // Inefficient before C++17 for complex data types because it creates a temp and then assigns it

    // Direct initilization
    int c(2);

    // List initilization (preffered)
    int d{3}; // Modern way of initiliation. Disambiguates from copy assingment and function related operations
    // Disallows narrowing conversions
    int e = 1.1; // Compiles with e being 1
    int f(1.1); // Compiles with f being 1
    // int g{1.1}; Compilation error
    
    // Zero initiliation
    int g{}; // Value of g is 0 for int. For other types, whatever is defined as "zero"

    // Multi variable initilization
    int h, i = 1; // h is not initilialized to 1 here
    // Preferred
    int j{1}, k{1};
}
