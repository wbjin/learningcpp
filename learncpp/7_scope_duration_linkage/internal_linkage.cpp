/*
 * LearnC++ 7.6 Internal Linkage
 *
 * copmile with make internal_linkage
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Internal Linkage
 * Global variables or funciton identifiers visible within the scope of a translation unit. The thing you define is only accesssible within the translation unit you are in
 *
 * NOn-const global variables have external linkage by default, make it internal using static
 * (static is a type of storage class sepcifier, sets idetnfiers linkage and storage duration)
 *
 * ODR: Identifiers that are defined in different files and are internal objects are considered different entities. Even if two files have the same, it won't violate ODR
 * */

// Internal linkage globals, only accessible within this file
static int x {1};
const int y {2};
constexpr int z {3};

// func not accessible outside of this file
static int func() {
    return 1;
}

int main() {
}
