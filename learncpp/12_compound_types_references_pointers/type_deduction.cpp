/*
 * LearnC++ 12.14 Type deduction with pointers, references, and const
 *
 * copmile with make retu
 *
 * */

#include <iostream>
using std::cout, std::endl;

/*
* Auto drops const and references.
 *
 * If you want to make variable declared with auto const or a reference, you
 * have to add it explicitly
 * */

/*
 * Top level const
 * Applies to the object itself
 *
 * Low level cosnt
 * Applies ot the object being reference or pointed to
 * */

/*
 * Type deduction with reference
 * If the value you are initializing is a reference or a top level const or
 * both, it is dropped first and then reapplied depending on the what you put
 * around auto
 * */

/*
 * Type deduction with pointers
 * Type deduction does not drop pointers but you can explicitly add them with a
 * * on the auto
 * */

const int& retRef() {
    static const int num = 5;
    return num;
}

int* retPtr() {
    static int a;
    return &a;
}

int main() {
    const int a {1};
    auto b {a}; // b is not const, just an int
    b = 2; // No error
    
    auto c {retRef()}; // c is not a const reference, just an int
    c = 2; // No error
    
    const auto d {a};
    const auto& e {retRef()};
    // e = 2; // Error because it const in front of auto& but even without it
    // because it is a reference to a cosnt

    const int f {a}; // top level cosnt because it is applying to the object itself
    const int &ref{f}; // low level const because the const is applied to the
                       // object being reference (f)
    const int *const ptr{&f}; // both. first const is low level (applied to f)
                              // and second const is top level (applied to ptr)
    
    auto g {retRef()}; // ref dropped, const dropped
    const auto h {retRef()}; // ref dropped, const dropped, const reapplied
    auto &i{retRef()}; // ref dropped, const not dropped because it is low level
                       // (const is for num), ref reapplied
    const auto &j{retRef()}; // ref dropped const, not dropped, ref reapplied

    auto k {retPtr()}; // int*
    auto* l {retPtr()}; // int*
    auto m {*retPtr()}; // int
    // auto* n {*retPtr()}; // compiler error, now n has to be a pointer
}
