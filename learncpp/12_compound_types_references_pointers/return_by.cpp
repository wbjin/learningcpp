/*
 * LearnC++ 12.12 Return by reference and return by address
 *
 * copmile with make return_by
 *
 * */

#include <iostream>

/*
 * Similar to passing by value, returning a large object by value means you have
 * to copy it which can be expensive.
 * */

/*
 * Return by reference
 * Returns a reference that is bound to the object being returned. Avoids making
 * copy of the return value. It is important the object being returned still
 * exists after the function returns. This means it can't be a variable local to
 * the function.
 *
 * When making a reference with a rvalue, it extends the lifetime of the temp
 * rvalue to match the liftime of the reference. This doesn't happen across
 * function boundaries so you cannot return a rvalue by reference as it will be
 * a dangling reference
 *
 * It's okay to return reference parameters by reference. When you pass in an
 * object by reference, the object is still in scope after the function returns
 * so it won't be a dangling reference.
 *
 * It's okay for an rvalue passed by const reference to be returned by const
 * reference. rvalues are only destroyed after the full expression in which they
 * are created is executed. It will still be in scope by the time the function
 * returns.
 *
 * You can modify a value returned by reference and it will modify that value.
 * */

/*
 * Return by address
 *
 * Works almost identical to pass by reference. Except you can return nullptrs
 * to express no valid return
 * */

const std::string& return_reference() {
    static const std::string ret {"Hi this is a reference string"};
    return ret;
}

const std::string& dangling_reference() {
    const std::string ret {"Hi this is a dangling reference string"};
    return ret;
}

const std::string& reference_rvalue(const std::string& r) {
    return r;
}

int& max(int& x, int& y) {
    return (x > y) ? x : y;
}

using std::cout, std::endl;

int main() {
    const std::string& str = return_reference();
    cout << str << "\n";

    const std::string& dangling_str = dangling_reference();
    cout << "Undefined behavior: " << dangling_str << "\n";

    std::string a {return_reference()}; // This creates a by making a copy of the return_reference
    
    std::string b {reference_rvalue("reference rvalue")};
    cout << b << "\n";
    
    int c {1};
    int d {2};
    max(c, d) = 3;
    cout << "c: " << c << "\n";
}
