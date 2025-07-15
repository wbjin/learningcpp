/*
 * LearnC++ 15.10 Ref qualifiers
 *
 * copmile with make friend
 *
 * */

#include <iostream>
#include <string_view>

using std::cout, std::endl;

/*
 * Returning member variables by reference can be dangerous because it can
 * create dangling references when the returned object is an rvalue.
 * */

/*
 * Ref qualifiers allows you to overload a member function based on whether it
 * is an lvaue or an rvalue.
 *
 * Non ref qualified overloads and ref qualified overloads can't coexist. If only lvalue qualified function exists, it will accept rvalue or lvalue. Use = delete if you want to disable a function for a specific ref overload.
 * */

class Example {
public:
    Example(std::string_view name) : _name{name} {}
    const std::string& get() { return _name; }

    const std::string& get_ref() const & { return _name; } // match with lvalue objects
    const std::string get_ref() const && { return _name; } // match with rvalue objects
private:
    std::string _name{};
};

Example create(std::string_view name) {
    Example e{name};
    return e;
}

int main() {
    cout << create("hi").get() << "\n"; // Fine because rvalue return is used in the expression

    const std::string &name{create("hi").get()}; // dangling reference, create("hi") is destroyed
                                                    // after expression
    cout << name << "\n"; // undefined

    const std::string& safe {create("hi").get_ref()}; // by value becasue create("hi") is lvalue
    cout << safe << "\n";
}
