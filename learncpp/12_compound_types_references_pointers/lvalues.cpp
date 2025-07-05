/*
 * LearnC++ 12.3 Lvalue referneces
 *
 * copmile with make lvalue
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Lvalue reference
 * Alias to an existing lvalue (variable)
 *
 * lvalue reference is a type. int&  is a "reference to int"
 *
 * References generally only bind to the matching reference type. You can't make
 * double& be a reference to an int
 *
 * You can't change what a reference is a reference to after initialization
 *
 * Dangling references: Accessing a reference to a destroyed object is undefined
 * behavior
 *
 * References aren't actually objects, often times the compiler replaces the
 * reference variable with the actual variable when it can. It doesn't occupy
 * storage so you can't do things you would be able to do with an object like a
 * reference to a reference. If you want a reference that can be reseated, use
 * std::reference_wrapper
 *
 * Lvalue references to const
 * You can't have a normal reference int& to a const int. It needs to be a const
 * int&.
 *
 * Lvalue references to const can bind to different types as long as their is an
 * implicit conversion between the types. The compiler will create a temporary
 * object of the reference type and bind to the temp object. This means that any
 * modification to the original variable will not be seen by the const reference
 * beecause you are binding to a temp.
 *
 * Const references bound to temp objects extends the lifetime of the temp
 * object to match the reference.
 *
 * Constexpr lvalue references
 * Allows the reference to be used in a constexpr. You can only constexpr
 * references to statics and globals
 * */

/*
 * Pass by const lvalue reference
 *
 * Remember that const lvalue references can bind to any type where there is a
 * valid conversion. When we pass by reference, we often don't want the copy to
 * happen so it is important to note that if you pass a type that is different
 * from the const lvalue reference of the function, there will be an implicit
 * copy which might not be what you want.
 * */

/*
 * Pass by value vs  reference
 *
 * Take into consideration the size of the object and setup costs (opening a
 * file, allocating memory, etc...) when you have to make a copy
 *
 * Binding a reference takes around the same time as copying a fundamental type
 *
 * consider cost of using function parameter. When function is called, compiler
 * may place passed by value arg into CPU register rather than RAM (if it is
 * small enough). However, when reference parameter is used, theres extra step
 * where program must first directly access storage location of the binded
 * variable (RAM instead of register)
 *
 * Compilers can be try more optimizations with pass by value. With pass by
 * reference, there is the chance of aliasing so compiler has to be more
 * conservative.
 *
 * Consider the following rule
 * Cheap to copy aka If object uses 2 * size of memory address and no cost of
 * setup (32 bit vs 64 bit systems) => Pass by value Expensive to copy => Pass
 * by reference
 * */

// Function-like macro that evaluates to true if the type (or object) is equal
// to or smaller than the size of two memory addresses
#define isSmall(T) (sizeof(T) <= 2 * sizeof(void*))

void func(const int& x) {}

int main() {
    int a {0};
    int& b{a};
    int& c{b}; // c is not a reference to a reference, it is actually a reference to a
    
    const int d{1};
    const int& e{d};
    const int& f{a}; // legal, f is a non-modifiable reference to a
    
    const int& g{2}; // Legal, temp object is created with rvalue and it is a reference to that 
    
    char h{'h'};
    // int& i{h}; // Illegal
    const int& i{h}; //Legal

    const int& j{3}; // Temp object holding 3 has same lifetime as j (end of function)
    
    //constexpr int& k{a}; // compile error because a is not global or static
    static int G = 0;
    constexpr int& k{G};

    static const int G_2 = 1;
    constexpr const int& l{G_2}; //constexpr reference to const needs the const keyword
   
    double m{0.6};
    func(m); // Implicit copy from double m to temp int which might not be what you want
    
    cout << isSmall(int) << "\n";
    cout << isSmall(double) << "\n";
}

