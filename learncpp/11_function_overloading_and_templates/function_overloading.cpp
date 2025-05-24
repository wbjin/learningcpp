/*
 * LearnC++ 11.2 Function overload differentiation
 *
 * copmile with make function_overloading
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Functions can be overloaded on the number of parameters, thet type of parameters, const or volatile qualifiers in member functions
 *
 * type aliases and type defs aren't considered. It will overload to the underlying type
 *
 * Const qaulifier is not considered for free functions
 *
 * Overload resolution: If function argument types match exactly with the function signature, easy to detect which function to overload to.
 * However, if the types are different, it can be hard to determine who to resolve to. 
 * The compiler tries different type conversions and checks for matching overloaded functions
 * 1. No match. Compiler moves on to next step
 * 2. A single match. This function considered best match.
 * 3. More than one. Compiler issues ambiguous match error. 
 *
 *
 * The compiler first looks for exact type matches.
 *
 * Then it does trivial conversions.
 * lvalue to rvalue conversions, qualification conversions, non-reference to reference conversions.
 * Trivial matches are considered exact matches. If there are ambiguous matches there is a copmile warning. 
 * In the following, if func2(int x) was defined, it would have been a compile error because of ambigous matching with func2(const int x)
 *
 * Then it does numeric promotion to look for matches.
 *
 * Then it does numeric conversion to look for mtaches
 * 
 * Then it does user defined conversions.
 *
 * Then it looks for matching functions with elipses
 *
 * Then it uses a compilation warning
 * */

void func(int x) {
    cout << "int\n";
}

void func(double x) {
    cout << "double\n";
}

// void func2(int x) {
//
// }

void func2(const int x) {
    cout << "const int\n";
}

void func2(const double& x) {
    cout << "const double&\n";
}

void func3(double) {
    cout << "double\n";
}

int main() {
    func(0); // Matches with func(int) because exact match
    func(3.1); // Matches with func(double) because exact match
    
    int x {1};
    double d {3.4};
    func2(x); // Trivally converted to const int
    func2(d); // Trivially converted to const double&

    func('a'); // Promoted to match int
    func(true); // Promoted to match int
    func(1.1f); // Promoted to match double
    
    func3('a'); // Matches with func3(double) after conversion
}

