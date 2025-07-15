/*
 * LearnC++ 15.8 Friend non-member functions
 *
 * copmile with make friend
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * Friend declaration
 *
 * Class that declares some other class as a friend lets that other class access
 * its private member variables and methods. Friend is always declared by the
 * class that is giving access.
 *
 * A friend can be a function, class, or a class methdod
 *
 * A friend class doesn't have access to the this pointer of the class that
 * gives access. The class that gives access does not have access to the friend
 * classes private methods/members.
 *
 * We can also give it access to a specific method of a function. This is a
 * little trickier because the class accessing the method has to be declared
 * before the class giving friend. The method also has to be defined after the
 * class giving friend.
 * */

class Example; // Forward declar for function signature

class FriendRestricted {
public:
    FriendRestricted() {}

    void print_example(const Example& e); // declare signature of function
};


class Example {
public:
    Example(int x) : 
        _x(x) {}

    friend void print(const Example&); // It doesn't matter if it is in public or private
    friend class Friend; // acts as a forward declaration
    friend void FriendRestricted::print_example(const Example&);
private:
    int _x;
};

class Friend {
public:
    Friend() {}

    void print_example(const Example& e) {
        cout << e._x << "\n";
    }
};

void FriendRestricted::print_example(const Example& e) { // Define after Example
    cout << e._x << "\n";
}

void print(const Example& e) {
    cout << e._x << "\n"; // can access private variable
}

int main() {
    Example a {2};
    print(a);

    Friend b {};
    b.print_example(a);

    FriendRestricted c{};
    c.print_example(a);
}
