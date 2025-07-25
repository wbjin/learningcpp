/*
 * LearnC++ 19.4 Pointers to pointers and dynamic multidimensional arrays
 *
 * copmile with make pointers
 *
 * */

#include <iostream>

using std::cout;

/*
 * Void pointer
 *
 * Void pointers can point to any type. However you can't dereference them
 * directly, you have to cast them to a type first.
 * */

int main() {
    int a {1};
    int* b {&a};
    int** c {&b};

    // int d {&&a}; // Can't do this &a is a rvalue and you can't get the
    // address of an rvalue

    int **d {new int*[10]}; // array of pointers

    // 2d array

    int size {4};
    int(*e)[5]{new int[size][5]}; // e is a pointer to an array of 5 ints, only
                                  // works if the second dimension is constexpr
    delete[] e;

    // IF you want dynamic 2d array 
    int **f {new int*[size]};
    for (int i {0}; i < size; ++i) {
        f[i] = new int[size];
    }
    // Deallocate with a loop as well
    for (int i {0}; i < size; ++i) {
        delete[] f[i];
    }
    delete[] f;

    void* g {&a};
    // *g; // error
    cout << *static_cast<int*>(g) << "\n"; // cast to an int* first

    void* h { new int{5} };

    delete static_cast<int*>(h); // also have to cast to deallocate
}
