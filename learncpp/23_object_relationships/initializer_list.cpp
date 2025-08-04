/*
 * LearnC++ 23.7 std::initializer_list
 *
 * copmile with make initializer_list
 *
 * */

#include <iostream>
#include <initializer_list>

using std::cout;

/*
 * initializer_list
 *
 * Initializer lists are converted to an object of std::initializer_list. It
 * takes a template parameter. It is like std::string_view in that it is a view
 * and copying it doesn't copy the elements in the list.
 *
 * Initializer list doesn't provide an indexing [] operator. Instead you can
 * loop through it with range iterator or use begin() and an offset based on the
 * return of begin().
 *
 * If you define a cosntructor that takes in a list_initializer, implement
 * overloaded list assignment operator, deep copy copy assignment operator, or
 * delete copy assignment operator.
 * */

class Arr {
public:
    Arr(int length) : _length{length}, _data{new int[length]} {}

    Arr(std::initializer_list<int> list) : Arr(list.size()) {
        std::copy(list.begin(), list.end(), _data);
    }

    ~Arr() {
        delete[] _data;
    }

    friend std::ostream& operator<<(std::ostream& out, const Arr& arr) {
        out << "Arr{";
        for (int i {0}; i < arr._length-1; ++i)
            out << arr._data[i] << ",";
        out << arr._data[arr._length-1];
        out << "}";
        return out;
    }

private:
    int _length {};
    int* _data {};
};

int main() {
    Arr a {1, 2, 3, 4, 5};
    cout << a << "\n";

    std::initializer_list<int> b {1, 2, 3, 4};
    for(auto i {std::begin(b)}; i != std::end(b); ++i)
        cout << *i << "\n";
}
