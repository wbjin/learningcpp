/*
 * LearnC++ 17.1 Introduction to std::array
 *
 * copmile with make move
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * std::array
 *
 * Fixed size container that is an aggregate.
 *
 * A std::array marked as const will make all elements in the array const. Try
 * to mark std::array as constexpr whenever possible.
 *
 * The compiler can deduce the template argument type with CTAD from the list
 * initializer. You can omit the <type, size> but you can't omit partially.
 * (can't have) <type> or <size>
 *
 * [] does no bounds checking, .at() does runtime bounds checking, std::get()
 * does compile time bounds checking for a constexpr index.
 * */

/*
 * Passing and returning std::array
 *
 * Need to specify type and size of function argument to a function. You can use
 * function templates to accept std::array of any type and size.
 * */

void func(const std::array<int, 5>&) {}

// has to be std::size_t because std::array is defined with std::size_t. Can
// also use auto
template<typename T, std::size_t N>
void func_any(const std::array<T, N>& arr){
    std::cout << std::get<1>(arr) << "\n"; // static assert of length
}

int main() {
    std::array<int, 5> a {}; // five 0s
    std::array<int, 5> b {0, 1, 2, 3, 4};
    std::array<int, 5> c {0, 1, 2}; // c[3] c[4] are value init
    //std::array<int, 5> d {0, 1, 2, 3, 4, 5}; // Error if too many
    const std::array<int, 5> e {0, 1, 2, 3, 4};
    // e[0] = 1; // Error

    constexpr std::array f {0, 1, 2, 3, 4}; // deduced to <int, 5>
    constexpr size_t g = f.size(); // size is constexpr so can use in constexpr
    constexpr int h{g + 1};        // not narrowing conversion because constexpr
                                   // std::size_t can be converted to int

    cout << std::get<3>(f) << "\n";
    
    func(f);

    constexpr std::array i {"hi", "hello"};
    func_any(f);
    func_any(i);
}
