/*
 * LearnC++ 15.5 Class templates with member functions
 *
 * copmile with make class_templates
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Class templates
 *
 * Functions inside class definition take parameter declaration belonging to
 * class.
 *
 * We don't need CTAD because we have explicit constructors that the compiler
 * can use to match types
 *
 * The injected class name is the shorthand for a fully templated name. For
 * example, using Pair in the class declaration of Pair<T> is the injected class
 * name.
 *
 * For member functions with class Templates, compiler needs both class
 * definition and template member function definition so typically put both
 * class and its member function in same location. This might mean class
 * declaration and definition might need to happen in header files.
 * */

template <typename T>
class Pair {
public:
    Pair(const T& first, const T& second) :
        _first{first},
        _second{second} {}

    void print();

private:
    T _first;
    T _second;
};

// Functions outside the class declaration need the template parameter again
template <typename T>
void Pair<T>::print() {
    cout << _first << "," << _second << "\n";
}

int main() {
    Pair p {1, 2};
}
