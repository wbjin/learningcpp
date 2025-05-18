/*
 * LearnC++ 10.8 Type deduciton 10.9 Type deduction for functions
 *
 * copmile with make type_deduction
 *
 * */
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using std::cout, std::endl;

/*
 * Type deduction: Allows compiler to deduce the type of an object from the object's initializer. Invoked with auto keyword
 *
 * You can also put the auto keyword in a functions returnt type. All retuyrn statements must returnt he same return type
 *
 * Functions with auto return type must be fully defined before use. (forward declaration isn't enough)
 *
  * */

auto func(int x, int y) {
    return 1 + 2;
}

// You can use auto keyword to do a trailing return type syntax
auto complex_return() -> std::vector<std::unordered_map<std::string, std::unordered_set<std::string_view>>> {
    return {};
}

int main() {
    const int a {1};
    auto b {a}; // b is no longer const
    const auto c {a}; // c is const 

    using namespace std::literals;
    auto d {"Hello"}; // Will be const char* and not std::string
    auto e {"Hello"s}; // Will be std::string
    auto f {"Hello"sv}; // Will be std::string_view
    
    constexpr int g {1};
    auto h {g}; // int, no const
    const auto i {g}; // const int
    constexpr auto j {g}; // const int
}

