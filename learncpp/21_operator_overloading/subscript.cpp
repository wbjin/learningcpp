/*
 * LearnC++ 21.9 Overloading the subscript operator
 *
 * copmile with make subscript
 *
 * */

#include <iostream>
#include <utility>

using std::cout;

/*
 * Overloading operators
 *
 * Three ways to overload operators: member function, friend function, and
 * normal function way.
 * */

/*
 * Subscript operator
 *
 * Overload the [] for a type.
 * */

class MyArray {
public:
    MyArray() = default;

    // Return by reference to allow modifying
    // If you don't want to rewrite the indexing, you can call the const []
    // operator with *this as a const and remove the const with const_cast
    int& operator[](int index) {
        return const_cast<int&>(std::as_const(*this)[index]);
    }

    const int& operator[](int index) const {
        return _arr[index];
    }

private:
    int _arr[10] {};
};

int main() {
    MyArray my_array {};

    my_array[1] = 1;
    cout << my_array[1] << "\n";
}
