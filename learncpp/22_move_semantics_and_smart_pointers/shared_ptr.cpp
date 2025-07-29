/*
 * LearnC++ 22.6 std::shared_ptr
 *
 * copmile with make shared_ptr
 *
 * */

#include <iostream>
#include <memory>

using std::cout;

/*
 * std::shared_ptr
 *
 * Multiple smart pointers co-own a resource. Multiple std::shared_ptr can point
 * to the same resource. It keeps track of number of pointers to the resource
 * and as long as one is point to it, the resource will not be deallocated. Once
 * the last pointr goes out of scope, the resource is deallocated.
 *
 * Shared pointers should be created from other shared pointers and not the
 * underlying resource. Pass in the shared pointer that you want to share
 * ownership with.
 *
 * Use std::make_shared<>() to initialize a shared_ptr
 *
 * std::shared_ptr contains a pointer to the underlying resource as well as a
 * pointer to a control block which is a dynamically allocated object that
 * tracks how many shared_ptrs are pointing at the resource. std::make_shared
 * can optimize this control block memory usage by using a single memory
 * allocation. shared_ptr constructor allocates it separately. This is also why
 * you need to pass in another shared_ptr to initialize a shared_ptr. The two
 * shared_ptrs need to point to the same control block it won't if you pass in
 * the underlying resource.
 *
 * shared_ptr can be created from unique_ptrs using move semantics taking in a
 * rvalue to a unique_ptr. Ownership will be transferred to the shared_ptr.
 * However, std::shared_ptr can't be converted to unique_ptr.
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
    Resource* a {new Resource{}};
    std::shared_ptr<Resource>b {a};
    {
        std::shared_ptr<Resource>c {b};
        // c goes out of scope
    }
    cout << *b << "\n"; // original resource a is still available
}
