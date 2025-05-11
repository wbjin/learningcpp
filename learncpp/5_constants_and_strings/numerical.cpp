/*
 * LearnC++ 5.3 Numerical systems
 *
 * copmile with make numerical
 *
 * */
#include <bitset>
#include <format> // C++20
#include <iostream>

using std::cout, std::endl;

int main() {
    int big_number {1'231'123'111};
    int binary_big_number {0b1011'1111'1111'0100'0000};

    // Output in different formats
    cout << std::hex << big_number << "\n";
    cout << std::oct << big_number << "\n";
    cout << std::dec << big_number << "\n";

    // Output in binary, 8 means 8 bits
    std::bitset<8> binary{0b1011'1101};
    cout << binary << "\n";

    cout << std::format("{:b}\n", binary_big_number);
    cout << std::format("{:#b}\n", binary_big_number);
}
