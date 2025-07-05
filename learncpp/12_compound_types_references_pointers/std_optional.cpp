/*
 * LearnC++ 12.15 std::optional
 *
 * copmile with make std_optional
 *
 * */

#include <functional>
#include <iostream>
#include <optional>
using std::cout, std::endl;

/*
 * std::optional
 *
 * Class template that implements an optional value. You can check if the object
 * is an optional or an actual value in an if statement or with
 * std::optional.has_value().
 *
 * You can use this for return values of a function that can have null options
 * if parameters are invalid.
 *
 * You can also use it to have optional function parameters. Instead of a
 * pointer that can take in a nullptr, use std::optional.
 * */

/*
 * std::optional doesn't support references directly but you can use std::reference_wrapper
 * */

std::optional<int> optional(bool which) {
    if (which)
        return 1;
    return std::nullopt;
}

void print(std::optional<int> num = std::nullopt) {
    if (num)
        cout << "Num: " << *num << "\n";
    else
        cout << "null\n";
}

void print_with_reference(std::optional<std::reference_wrapper<int>> num = std::nullopt) {
    if (num)
        cout << "Num: " << *num << "\n";
    else
        cout << "null\n";
}


int main() {
    std::optional<int> a = optional(false);
    if (a)
        cout << "a has value " << *a << "\n";
    else
        cout << "optional\n";
    
    print();
    print(1);

    int num = 5;
    print_with_reference();
    print_with_reference(num);
}
