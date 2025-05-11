/*
 * LearnC++ 5.8 Introduction to std::string_view
 *
 * copmile with make constexpr
 *
 * */
#include <iostream>
#include <string_view>

using std::cout, std::endl;

/*
 * std::string_view
 * Read only access to an existing string (C-style string, std::string, or another std::string_view)
 *
 * std::string is an owner. As an owner, you are responsible for allocating, managing, and deallocating
 * std::string_view is a view. You don't have to do allocation, management, deallocating. However, what you are viewing may change unexpectedly
 *
 * std::string_view is used only in read only situations
 * Only use it when the object you are referencing is still in scope and won't be modified
 * Don't allocate with std::string literal, C-style strings are embedded into the actual code while std::string is not, lead to dangling view
 * */

/*
 * std::string usage
 * Modifiable, store a return value of a function
 *
 * std:string_view
 * Read only access, need a symbolic constant to a C-style string
 * */

void func(std::string_view sv) {
}

int main() {
    // Read only access to the stirng literal
    std::string_view sv { "hi" };

    // Function parameter does not make a copy of the actual underlying string
    func(sv);

    // You can't implicitly convert string_view to string
    // std::string str = sv;
    std::string str = static_cast<std::string>(sv); // Do it explicitly
    
    // Assigning new string doesn't modify the old string in any way, it just chanes what the string_view is referencing
    sv = "different string";

    // Constexpr doesn't work on std::string
    // constexpr std::string constexprstring { "Won't work" };
    constexpr std::string_view constexprstringview { "Will work" };

    std::string a { "Hi" };
    std::string_view b { a };
    a = "Hihihihi"; // b invalidated
    cout << b << "\n"; // Undefined behavior
    b = a; // Revalidate
    cout << b << "\n";

    std::string_view c { "abcdefg" };
    // string_view allows you to change the view (what part of the string you are looking) at
    c.remove_prefix(1); // Removes 'a' from the view, doesn't actually change the string
    cout << c << "\n";
    c.remove_suffix(3);
    cout << c << "\n";
}
