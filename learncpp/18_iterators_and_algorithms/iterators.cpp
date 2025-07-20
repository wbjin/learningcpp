/*
 * LearnC++ 18.2 Introduction to iterators
 *
 * copmile with make iterators
 *
 * */

#include <array>
#include <iterator>
#include <iostream>

using std::cout, std::endl;

/*
 * Range based for loops use iterators underneath the hood.
 *
 * Alterting the data structure while having an iterator to it will invalidate
 * the iterator.
 * */

int main() {
    constexpr std::array a {1, 2, 3, 4, 5};

    for (auto it = a.begin(); it != a.end(); ++it) {
        cout << *it << "\n";
    }

    // can also sue std::begin() and std::end() for containers that support iterators
    for (auto it = std::begin(a); it != std::end(a); ++it) {
        cout << *it << "\n";
    }
}
