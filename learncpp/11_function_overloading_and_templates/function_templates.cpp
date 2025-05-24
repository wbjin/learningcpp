/*
 * LearnC++ 11.6 Function templates
 *
 * copmile with make funtion_templates
 *
 * */
#include <iostream>
#include <vector>

using std::cout, std::endl;

/*
 * Templates: Tells the compiler how to generate overloaded functions
 *
 * Function template (the primary template) is used to generate overloaded functions (instantiated functionss)
 *
 * The primary functino template can have placeholder types (tempalte types) for parameter, return, and types used in the function
 *
 * There are 3 different kinds of template parameters
 * Type template: Template parameter represents a type
 * Non-type template: Template parameter represents constexpr
 * Template Template: Template parameter represents a template
 * */

template <typename T> // T is a type template. typename keyword is interchangeable with class
T max(T x, T y) {
    return (x < y) ? y : x;
}

// For type template with non-obvious useage, more descriptive name
template< class T, class TCompare >
const T& max( const T& a, const T& b, TCompare comp ); // TCompare is a comparator for the type T

/*
 * Function template isntantiation is the creation of functions from templates.
 *
 * Implicit instantiation is when a a function is created due to a function call.
 *
 * Function instance (specialization) is when a function is created from a template
 *
 * You can tell the compiler to delete instantiation of certain arguments with function templates
 * */

template <>
std::string max(std::string, std::string) = delete;
const char* max(const char*, const char*) = delete;

template <typename T, typename U>
auto max2(T x, U y) { // The return type has to be auto because it can be T or U
    return (x < y) ? y : x;
}

/*
 * Function templates can also be overloaded. There can be multiple function templates of different types
 *
 * The selection of function templates follows the partil ordering of function tempaltes rules. 
 * The more restritive function template wil be preferred (only one type instead of two)
 * */
template <typename T>
auto add(T x, T y)
{
    return x + y;
}

// As of C++20 we could also use auto add(auto x, auto y)
template <typename T, typename U>
auto add(T x, U y)
{
    return x + y;
}

// As of C++20 we could also use auto add(auto x, auto y, auto z)
template <typename T, typename U, typename V>
auto add(T x, U y, V z)
{
    return x + y + z;
}

int main() {
    std::cout << max<int>(1, 2) << "\n";
    std::cout << max(1, 2) << "\n"; // Can also do this and have compiler figure out which function to use

    // std::cout << max<std::string>("ab", "ba") << "\n"; // Doesn't compile
    // std::cout << max("ab", "ac") << "\n"; // Also doesn't compile
    
    // std::cout << max(1, 1.11) << "\n"; // Compile error
    std::cout << max<int>(1, 1.1) << "\n"; // This compiles because max<int> we specify which type to instantiate.
    std::cout << max2(1, 1.11) << "\n";
    
    std::cout << add(1, 1) << "\n";
    std::cout << add(1, 1.2) << "\n";
    std::cout << add(1, 1.2, 1.5f) << "\n";
}

