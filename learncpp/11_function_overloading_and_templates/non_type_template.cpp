/*
 * LearnC++ 11.9 Non type templates
 *
 * copmile with make non_type_template
 *
 * */
#include <iostream>
#include <vector>

using std::cout, std::endl;

/*
 * Non type template parameter is a fixed type that serves as a placeholder for a constxpr value
 * It can be integral type, enumeration type, std::nullptr_t, floating point since 20,...
 *
 * For example, in std::bitset, you initialize by doing std::bitset<8> where 8 is the number of bits
 *
 * This is useful for function parameters that you want to be constexpr
 * */

// You can make a potentially expensive computationl function a template to make it compile time evaluated
template <int X>
int getCubed() {
    return X * X * X;
}

template <int N>
void print() {
    cout << N << "\n";
}

int main() {
    print<5>();
    cout << getCubed<5>() << endl;
}

