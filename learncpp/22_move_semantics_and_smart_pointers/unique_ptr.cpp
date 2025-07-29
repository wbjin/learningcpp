/*
 * LearnC++ 22.5 std::unique_ptr
 *
 * copmile with make unique_ptr
 *
 * */

#include <iostream>
#include <memory>

using std::cout;

/*
 * std::unique_ptr
 *
 * Used to manage dynamically allocated objects that is not shared by multiple
 * objects. std::unique_ptr completely owsn the object it manages.
 *
 * It implements move semantics and copy semantics are disabled. To move a
 * unique_ptr, you have to use copy semantics.
 *
 * Operator* returns a reference to underlying object, Operator-> returns a
 * pointer.
 *
 * std::unique_ptr can figure whether to use delete or delete[] so you can use C
 * style array with it.
 *
 * std::make_unique is preferred over creating a std::unique_ptr manually.
 * std::make_unique constructs an object specified inthe template type and
 * initializes with arguments passed into the function.
 *
 * Generally, don't use unique pointers by reference. You can return and pass as
 * an argument by value and use std::move according to invoke move semantics.
 *
 * Avoid allowing multiple unique_ptrs to manage one objecta and don't manually
 * delete the underlying resource of a unique ptr
 * */

class Resource {
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }

    friend std::ostream& operator<<(std::ostream& o, const Resource&) {
        o << "Resource{}";
        return o;
    }
};

int main() {
    std::unique_ptr<Resource> a {new Resource{}};
    std::unique_ptr<Resource> b {};

    cout << b << "\n"; // nullptr
    b = std::move(a);
    cout << a << "\n"; // nullptr after mvoe

    cout << *b << "\n";
    cout << b.get() << "\n";

    auto c {std::make_unique<Resource>()};
}
