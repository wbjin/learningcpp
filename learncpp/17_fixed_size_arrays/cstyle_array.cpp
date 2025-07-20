/*
 * LearnC++ 17.7 Introduction to C-style arrays
 *
 * copmile with make cstyle_array
 *
 * */

#include <iterator>
#include <iostream>

using std::cout, std::endl;

int main() {
    int a[] {1, 2, 3, 4, 5};
    cout << std::size(a) << "\n"; // returns size as an unsigned std::size_t
    // cout << std::ssize(a) << "\n"// returns size as a signed std::ptrdiff_t C++20 feature
}
