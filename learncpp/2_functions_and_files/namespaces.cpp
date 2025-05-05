/*
 * LearnC++ 2.9 Naming collisions and an introduction to namespaces
 *
 * copmile with make namespaces
 *
 * */
#include <iostream>

// using namespace std; Avoid this. Any identifier we make may conflict with the STL
using std::cout, std::endl;

// Global namesapce
void func() {
    cout << "Global func\n";
}
int x{5};

namespace custom {

void func() {
    cout << "Custom func\n";
}
int x{4};

}

int main() {
    func();
    cout << x << "\n";

    // Doesn't conflict with the globally scoped func because it is in its own namespace
    custom::func();
    cout << custom::x << "\n";
}
