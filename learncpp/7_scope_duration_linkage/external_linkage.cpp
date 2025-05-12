/*
 * LearnC++ 7.8 External linkage ad variable forward declarations
 *
 * copmile with make external_linkage
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * External linkage
 * Identifier can be seen from the file it is defined and other files. Can be used anywhere in program
 *
 * Use the extern keyword to make a global variable accessible outside of a file (see helper.cpp)
 *
 * TRY NOT TO USE extern non-const variables
 * */

// Forward declaration of variable in helper.cpp
extern int non_const;
extern const int global; // global is a constexpr in helper.cpp but it cannot be in here because the copmiler needs to know values of constexpr at compile time, not link time. This will be a runtime constant

// Forward declaration of function in helper.cpp
void func();

/*
 * Initialiation order of global variables
 * First phase: Static initialization. Global variables with constexpr intializers initialized. Global variables without initializers are zero-initialized.
 * Second phase: Dynamic initialization: Global variables with non constexpr initializers
 *
 * Basically, be careful about the order of initialization of global variables. Make sure nothing is ambiguous and it is not dynamic
 * */

int main() {
    func(); // Calls function defined in another file
    cout << non_const << endl;
    cout << global << endl;
}
