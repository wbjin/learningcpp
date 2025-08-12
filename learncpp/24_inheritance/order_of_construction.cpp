/*
 * LearnC++ 24.3 Order of construction of derived classes
 *
 * copmile with make order_of_construction
 *
 * */

#include <iostream>

using std::cout;

/*
 * Derived classes are really two parts. The base and the derived part. First,
 * the most base class at the top of the inheritance tree is cosntructed. Then
 * each child class is constructed in order
 * */

/*
 * When a class constructor is called,
 * 1) Memory for class is set aside
 * 2) Appropriate class constructor is called
 * 3) Member initializer list initalizes variables
 * 4) Body of constructor executes
 * 5) Return
 *
 * When a class constructor of a derived class is called
 * 1) Memory for derived class is set aside (enough for base and derived
 * classes)
 * 2) Base object is constructed fir using appropriate constructor,
 * default if no specification
 * 3) Member initializer list initializes variable
 * 4) Body of constructor executes
 * 5) Return
 * The base class constructor is called before any derived class constructor
 * Note that only non-inherited variables can be initialized in the intializer
 * list. This is because base classes are constructed first and for const and
 * reference variables, it has to be set at class construction and not after.
 *
 * The actual way to initialize base class member variables is to explicitly
 * call the base class constructor in the initializer list.
 * */

/*
 * Destructors are called in reverse order of construction.
 * If your base class has any virtual functions, destructor should also be
 * virtual. Udefined behavior otherwise.
 * */

class Base {
public:
    const int _i {};

    Base() {
        cout << "Base\n";
    }

    Base(int i) : _i {i} {};
};

class Derived : public Base {
public:
    int _j;

    Derived() {
        cout << "Derived\n";
    }

    Derived(int i, int j) : Base{i}, _j{j} {};
};

class Derived2 : public Derived {
public:
    Derived2() {
        cout << "Derived2\n";
    }
};

int main() { 
    Derived a {}; // Base printed first
    
    Derived2 b {};

    Derived c { 1, 2 };
    cout << c._i << " " << c._j << "\n";
}
