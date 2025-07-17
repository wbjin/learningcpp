/*
 * LearnC++ 16.12 std::vector<bool>
 *
 * copmile with make vector_bool
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * std::vector<bool>
 *
 * A special class template specialization for vectors with templtae argument
 * bool. The performance is highly dependent on implementation so it can be
 * better significantly better or worse. THe overhead can also be higher.
 *
 * std::vector<bool> optimization stores items as bits and might not be
 * contiguous in memory. It's usually not recommended.
 *
 * Instead use
 * std::bitset if you know the number of bits you need at compile time
 * boost::dynamic_bitset instead.
 * */

int main() {
    cout << "Size of vector<bool>: " << sizeof(std::vector<bool>) << "\n";
    cout << "Size of vector<int>: " << sizeof(std::vector<int>) << "\n";
}
