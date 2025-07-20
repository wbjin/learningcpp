/*
 * LearnC++ 18.3 Introduction to standard library algorithms
 *
 * copmile with make stl
 *
 * */

#include <array>
#include <iterator>
#include <iostream>

using std::cout, std::endl;

/*
 * std::find
 *
 * Begin iterator, end iterator, element to look for. Returns iterator to
 * element.
 * */

/*
 * std::find_if
 *
 * Find an element that matches some condition.
 * Begin iterator, end iterator, callable object. Returns iterator to element.
 * */

/*
 * std::count and std::count_if
 *
 * Count number of occurences of some element or some condition.
 * Begin terator, end iterator, element or callable object.
 * */

/*
 * std::sort
 *
 * Sort container with a specific rule.
 * Begin iterator, end iterator, comparator. By default sorts in increasing
 * order with less comparator.
 * */

/*
 * std::for_each
 *
 * Applies some operation to elements.
 * Begin iterator, end iterator, operation
 * */

int main() {
    constexpr std::array a {1, 4, 13, -3, 1, 31};

    auto b {std::find(std::begin(a), std::end(a), -3)};
    if (b != a.end())
        cout << "found: " << *b << "\n";

    auto c {std::find_if(std::begin(a), std::end(a), [](int x){return x < 0;})};
    if (c != a.end())
        cout << "< 0 found: " << *c << "\n";

    auto d {std::count(std::begin(a), std::end(a), 1)};

    cout << "number of occurences of 1: " << d << "\n";

    auto e {std::count_if(std::begin(a), std::end(a), [](int x){return x == 1 || x == 13;})};
    cout << "number of occurences of 1 or 13: " << e << "\n";


    std::array f {5, 2, 1, 3, 4, 2, -1, 16};
    std::sort(std::begin(f), std::end(f), [](int a, int b){
        return a > b;
    });
    for (auto i : f)
        cout << i << "\n";

    std::for_each(std::begin(f), std::end(f), [](int& x){
        x+=1;
    });
    for (auto i : f)
        cout << i << "\n";
}
