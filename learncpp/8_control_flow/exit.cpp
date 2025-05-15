/*
 * LearnC++ 8.12 Halts
 *
 * copmile with make exists
 *
 * */
#include <cstdlib>
#include <iostream>

using std::cout, std::endl;

void cleanup() {
    cout << "In cleanup\n";
}

int main() {
    // Register the function to be called when std::exit is called
    std::atexit(cleanup);
    std::exit(0);
}
