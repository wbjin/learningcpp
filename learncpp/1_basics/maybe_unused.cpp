/*
 * LearnC++ 1.4 Variable assignment and initilization
 *
 * copmile with make maybe_unused
 *
 * */
#include <iostream>

using std::cout, std::endl;

int main() {
    // [[maybe_unused]] attribute on a variable will make the copmiler not complain if a variable is not used
    int a{0};
    [[maybe_unused]] int b{0};
    // Copmiler will complain about a not being used but not for b
}
