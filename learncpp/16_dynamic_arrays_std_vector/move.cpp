/*
 * LearnC++ 16.5 Returning std::vector, and an introduction to move semantics
 *
 * copmile with make move
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Move semantics
 *
 * Transfer of ownership from one object to another. Usually, switching around
 * pointers. The object we are moving from also doesn't have any data to
 * destroy. Move semantics is the rules that determine how the data from one
 * object is moved to another.
 *
 * Move semantics are invoked when
 * - Type of object supports move semantics (implements move semantics)
 * - Object is being initialized with rvalue of same Type
 * - Move isn't elided
 *
 * You can return std::vector by value because that is an rvalue return and
 * vector supports move semantics. Return value will be moved instead of copied.
 * */

std::vector<int> get() {
    return {1, 2, 3}; // rvalue return
}

std::vector<int> get2() {
    std::vector<int> a {1, 2, 3};
    return std::move(a); // explicitly invoke move
}

int main() {
    auto a {get()}; // move semantics

    auto b {get2()}; // move semantics
}
