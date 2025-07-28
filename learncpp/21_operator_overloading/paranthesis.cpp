/*
 * LearnC++ 21.10 Overloading the parenthesis operator
 *
 * copmile with make parenthesis
 *
 * */

#include <iostream>

using std::cout;

/*
 * () operator
 *
 * Allows you to vary by type and number of parenthesis it takes. Needs to be a
 * member function.
 *
 * You can make functors with the () operator overload. Functors are objects
 * that operate like functions.
 * */

class Matrix {
public:
    Matrix() = default;

    int& operator()(int row, int col) {
        return _matrix[row][col];
    };

    int& operator()() {
        return _matrix[0][0];
    }

private:
    int _matrix[10][10] {};
};

int main() {
    Matrix m {};
    m(1, 2) = 1;
    cout << m(1, 2) << "\n";
}
