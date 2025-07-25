/*
 * LearnC++ 19.1 Dynamic memory allocation with new and delete
 *
 * copmile with make new_delete
 *
 * */

#include <iostream>

/*
 * Operator new
 *
 * when new fails, it by default throws a bad_alloc exception. If you don't want
 * it throw exceptions, you can use std::nothrow
 *
 * from C++11
 * */

/*
 * Operator delete
 *
 * Deleting a nullptr has no affect
 * */

using std::cout;

int main() {
    int* a {new (std::nothrow) int};
    if (!a)
        cout << "new failed\n";
    delete a;
}
