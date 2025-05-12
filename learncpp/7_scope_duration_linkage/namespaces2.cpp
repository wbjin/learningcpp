/*
 * LearnC++ 7.2 User-defined namespaces
 *
 * copmile with make namespace2
 *
 * */
#include <iostream>

using std::cout, std::endl;

void print3() {
    cout << "print3\n";
}

// Declaration
namespace Foo {
void print();
void print2();
}

// Definition
namespace Foo {
void print() {
    cout << "print\n";
    // Will first look for functions that match the identifier in the same namespace
    print2();
    // Then will look for functions in global namespace
    print3();
}

void print2() {
    cout << "print2\n";
}
}

// Multiple namespaces of the same identifier are allowed. This makes it easy to extend a namespace without having it all in one file
namespace Foo {
void print4() {
    Foo::print(); // Can access namespace functions from other blocks with same idetnifier
}
}

// Nested namespaces. Goo is a namespace inside Foo
namespace Foo::Goo {
void print5() {
    cout << "print5\n";
}
}

// Accessing nested namespaces can be annoying so you can create namespace aliases
namespace Nested = Foo::Goo;

// Unnamed namespace
// Treated as if it is part of the parent namesapce (in this case is global because no other namespace defined)
// Useful for limiting use of function in this one file, there is itnernal linkage
namespace {
void print6() {
    cout << "print6\n";
}
}
// the same as 
// static void print6() {
//     cout << "pritn6\n";
// }

// Inline namespaces are used to version
inline namespace V1 // declare an inline namespace named V1
{
    void doSomething()
    {
        std::cout << "V1\n";
    }
}

namespace V2 // declare a normal namespace named V2
{
    void doSomething()
    {
        std::cout << "V2\n";
    }
}

int main() {
    Foo::print();
    Foo::print4();
    Foo::Goo::print5();
    Nested::print5();
    print6(); // Call the unnamed namespace
    V1::doSomething();
    V2::doSomething();
    doSomething(); // V1::doSomething
}
