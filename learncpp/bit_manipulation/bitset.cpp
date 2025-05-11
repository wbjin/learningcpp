/*
 * LearnC++ Bit flags and bit manipulation
 *
 * copmile with make bitset
 *
 * */
#include <iostream>
#include <bitset>

using std::cout, std::endl;

int main() {
    std::bitset<8> bits {};

    bits.set(0); // Set bit at position to 1
    bits.set(1);
    cout << "Bits " << bits << "\n";
    bits.reset(1); // Set bit at position 0

    cout << "Bit 1 " << bits.test(1) << "\n"; // Query whether or not bit is 0 or 1

    bits.flip(2); // Flip the bit at position
    
    // Bitset is the number of bytes needed to hold the bits rounded up to the nearest sizeof(size_t)
    cout << "Size of std::bitset " << sizeof(std::bitset<8>) << "\n";

    cout << "Number of bits " << bits.size() << "\n";
    cout << "Number of set bits " << bits.count() << "\n";
    cout << "Are all bits set " << bits.all() << "\n";
    cout << "Is any bit set " << bits.any() << "\n";
    cout << "No bits set " << bits.none() << "\n";

    std::bitset<4> b {0b1100};
    cout << "Original " << b << "\n";
    cout << "Right shift 1 " << (b >> 1) << "\n";
    cout << "Left shift 1 " << (b << 1) << "\n";
    cout << "Bitwise not " << ~b << "\n";
}
