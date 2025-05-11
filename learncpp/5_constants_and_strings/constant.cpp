/*
 * LearnC++ 5.1 Constant variables, 5.2 Literals
 *
 * copmile with make constant
 *
 * */
#include <iostream>
#include <string>

using std::cout, std::endl;

/*
 * Const qualifier is a type. If you put `const` on an object, the type of that object becomes `const <type>`
 *
 * Prefer constant variables over preprocessor macros. Macros are not scoped, it may cause a macro substitution elsewhere that you did not expect
 *
 * Aside:
 * `volatile` qualifier tells the compiler that the object can have its value changed at any time.
 * The value of an object can change between accesses even if there is no visible writes to them (maybe because it is multithreaded). Tells compiler to not optimizie anything involving it so you get the correct value
 * */

/*
 * Literals are values that are inserted directly into code.
 * */

int main() {
    // Const can be on either side of the type
    [[maybe_unused]] const int x {4};
    [[maybe_unused]] int const y {5};

    // Compiler error, const variables have to be initialized
    // const int a;

    int b = 5;
    int c = 5u;
    float d = 5.0f;
    long double e = 5.0L;
    float f = 5.0;
     
    // String literal are C-style strings. They are const objects created at the start of the program and exist for the entirety of the program
    const char* g = "string literal";
}
