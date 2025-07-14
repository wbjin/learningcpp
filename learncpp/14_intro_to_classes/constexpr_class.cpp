/*
 * LearnC++ 14.17 Constexpr aggregates and classes
 *
 * copmile with make constexpr_class
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Constexpr member functions
 *
 * Member functions can be made constexpr to be evaluated at either compile or
 * runtime.
 *
 * Aggregates implicitly support constexpr so you can make an aggregate a
 * constexpr when initializing.
 *
 * * If an object is an aggregate, it can be a literal type. A literal type is
 * any object for which it might be possible to create one within a constant
 * expression.
 * */

struct Pair
{
    int m_x {};
    int m_y {};

    constexpr int greater() const // can evaluate at either compile-time or runtime
    {
        return (m_x > m_y  ? m_x : m_y);
    }
};

class PairClass {
public:
    constexpr PairClass(int x, int y) :
        _x(x),
        _y(y) {}

    constexpr int greater() const // can evaluate at either compile-time or runtime
    {
        return (_x > _y  ? _x : _y);
    }
private:
    int _x;
    int _y;
};

int main() {
    Pair a {5, 6}; //compile time
    std::cout << a.greater() << "\n"; // runtime
                        
    // constexpr int b {a.greater()}; // Compiler error because a is not constexpr

    constexpr Pair c {5, 6};
    constexpr int d {c.greater()};
    cout << d << "\n";

    constexpr PairClass e {5, 6}; // If constructor doesn't have constexpr, doesn't work
}
