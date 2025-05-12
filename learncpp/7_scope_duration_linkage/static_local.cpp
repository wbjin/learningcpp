/*
 * LearnC++ 7.11 Static local variables
 *
 * copmile with make static_local
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Static keyword changes the lifetime of a variable local variable when appled
 *
 * Static local variables are created at the start of the program and destroyed at the end
 *
 * Retains state across calls to that function
 *
 * Scope is still within the block (not global), but retains state1
 *
 * Usefule for id generation
 * */

void increment() {
    static int counter {0}; 
    ++counter;
    cout << counter << "\n";
}

int main() {
    increment(); // 1
    increment(); // 2
    increment(); // 3
}
