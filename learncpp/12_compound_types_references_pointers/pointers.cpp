/*
 * LearnC++ 12.7 Introduction to pointers
 *
 * copmile with make pointers
 *
 * */

#include <algorithm>
#include <iostream>

using std::cout, std::endl;

/*
 * References and pointers achieve the same effect in different ways, bobth
 * allow you to indirectly access another object. With pointers you have to
 * explicitly get the address to point at, with references the address of and
 * dreferencing happens implicitly
 *
 * Some differences
 * - References need to be initialized
 * - References aren't objects, it doesn't actually take memory, pointers are
 * - References can't be chnanged to something else (without reference wrapper)
 * - References must always be bound to an object
 * */

/*
 * Pointer to const
 * Pointer that doesn't let you change the value it is pointing to
 * Can't change the value that the pointer is pointing to
 * */

/*
 * Const pointer
 * Pointer that cannot point to something else after initialization
 * */

/*
 * Const pointer to cosnt
 * A pointer that doesn't let you change the value you are pointing at and
 * doesn't let you point at anything else
 * */

/*
 * Prefer pass by const reference rather than pointer to const
 * A reference cannot be a null reference, pointers can be nullptrs.
 * Passing by pointer only allows lvalues to be passsed in, no r-values (temp
 * objects)
 * Passing by address allows optional arguments, this is when you would use it
 * pass by address instead
 * */

/*
 * Pass by address by reference
 * If you want to for some reason, change what a pointer is pointing to, you
 * have to pass the pointer as a reference. If you take a pointer by itself, it
 * copies the address
 * */

/*
 * Avoid NULL
 * NULL is a preprocessor macro that is implementation defined. It can be
 * defined as 0, 0L or (void*)0. This is a problem because function overloading
 * can happen where you intended to pass a NULL as a ptr but it was implicitly
 * cast to an integer
 *
 * Use nullptr instead
 * nullptr has type std::nullptr_t which is a type that can only hold nullptr
 * You can make a function that takes in a std::nullptr_t that only takes in a nullptr
 * */

/*
 * Everything is pass by value.
 * Underneath the hood, passing by referencing is implemented by the compiler
 * using pointers and passing by address. This means that ultimately, everything
 * is passingby value.
 * */

void nullify_ptr(int*& refptr) {
    refptr = nullptr;
}

int main() {
    const int a{1};
    const int* b{&a}; // Pointer to a const object
    // *b = 5; // Error
    
    int c{2};
    b = &c; // Can change what the pointer is pointing to 

    int d{3};
    b = &d; // Pointer to const can point to a non const value
    // *b = 4; // Still an error

    int* const e{&d}; // Const pointer
    *e = 4; // Can change value
    // e = &c; /Can't change what the variable is pointing to

    const int* const f{&c};
    // *f = 5; // Error
    // f = &d; // Error
    
    int g {1};
    int* ptr = &g;
    nullify_ptr(ptr);
    cout << ((ptr == nullptr) ? "nullptr\n" : "not nullptr\n");
}
