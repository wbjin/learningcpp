/*
 * LearnC++ 17.4 std::array of class types, and brace elision
 *
 * copmile with make object_array
 *
 * */

#include <array>
#include <iostream>

using std::cout, std::endl;

/*
 * You can put any object type in std::array.
 *
 * */

struct Example {
    int x {};
    int y {};
};

int main() {
    // Using CTAD to deduce the type as Example and length as 2
    constexpr std::array a {
        Example{1, 2},
        Example{2, 3}
    };

    // However can't do this. Compiler will only use the first struct {1, 2} and
    // initialize the first C-style array element iwth that. That other indices
    // will be zero initialized. However because more initializers are provided
    // there is an error.
    // constexpr std::array<Example, 2> b {
    //     {1, 2},
    //     {2, 3}
    // };
    
    // Instead add another curly brace before initializer list
    constexpr std::array<Example, 2> c {
        {
            {1, 2},
            {2, 3}
        }
    };
}
