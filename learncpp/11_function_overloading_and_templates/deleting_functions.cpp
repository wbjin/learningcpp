/*
 * LearnC++ 11.4 Deleting functions
 *
 * copmile with make deleting_functions
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * If we don't want certain functions to not be callable, we cant use = delete. Compilation will error if a match to a deleted functino happens
 * */

void func(int) {
    cout << "int\n";
}

void func(double) = delete; // Maybe we don't want match to a double for this function

/*
 * Instead of deleting functions that we don't want, we can also specify only the ones that we do want
 * */

void func2(int) {
    cout << "int\n";
}

void func2(double) {
    cout << "double\n";
}

template<typename T>
void func2(T) = delete;

int main() {
    func(1);
    //func(1.1); // compilation error

    func2(5);
    func2(5.5);
    // func2('a'); // Compilation error
}

