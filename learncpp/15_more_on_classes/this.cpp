/*
 * LearnC++ 15.1 The hidden this pointer and member function chaining
 *
 * copmile with make this
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Member function chaining
 *
 * Return *this by reference to chain functions together.
 * */

class Example {
public:
    Example() = default;

    Example(int x) : _x(x) {}

    Example& add(int value) {
        _x += value;
        return *this;
    }

    void reset() {
        *this = {}; // Easy way to reinitialize a class
    }

    void print() {
        cout << _x << "\n";
    }

private:
    int _x {};
};

int main() {
    Example a {1};

    a.add(2).add(3).add(4); // Instead of three adds separately.
    a.print(); 

    a.reset();
    a.print();
}
