/*
 * LearnC++ 20.1 Function pointers
 *
 * copmile with make function_pointers
 *
 * */
#include <iostream>
#include <functional>

using std::cout;

/*
 * Function pointers
 *
 * Store the address of a functino definition.
 *
 * Default arguments don't work for functions called through function pointers.
 * Default argument rewrite happens at compile time but at for function
 * pointers, not known at compile time.
 *
 * You can pass callback functions as an argument into a function so that it
 * can be called by another function.
 * */

/*
 * std::function
 *
 * An easier way to define a functino pointer.
 * */

int foo() {
    return 1;
}

int foo2() {
    return 2;
}

void foo3(int) {}

int add_1(int x, void (*callback)(int)) {
    int y {x + 1};
    callback(y);
    return y;
}

int add_2(int x, std::function<void (int)> callback) {
    int y { x + 2};
    callback(y);
    return y;
}

void callback(int x) {
    cout << "in callback: " << x << "\n";
}

int main() {
    // Functions referenced without () will become function pointers.
    cout << foo << "\n";

    int (*a)() = &foo; // a is a function pointer to any function that takes in
                      // no parameters and returns an int
    cout << a() << "\n";
    a = &foo2;
    cout << a() << "\n";

    // a = &foo3; // error return and parameter types don't match

    void (*b)(int) {&foo3};

    int c {add_1(2, callback)};

    int d {add_2(2, callback)};
}
