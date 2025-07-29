/*
 * LearnC++ 22.3 Mvoe constructors and move assignment
 *
 * copmile with make move
 *
 * */

#include <iostream>

using std::cout;

/*
 * Move constructor and move assignment
 *
 * Move the ownership of the resouces of one object to another. Copy copies the
 * resources, move moves so the the thing you move from will be left in a state
 * that is essentially newly initialized.
 *
 * Move takes a nonconst rvalue reference to an object. Copy takes a cosnt
 * lvalue reference to an object. They should be marked as noexcept which means
 * it will not through an exception
 *
 * Move constructor and assignment is called when they have been defined and the
 * argument for construction is an rvalue. Copy is when the argument is a
 * lvalue.
 *
 * The compiler creates implicit move constructor and assignment if there are no
 * user declared copy semantics, no user declared move semantics, and no user
 * declared destructore. This implicit move cosntructor uses the move assingment
 * if it exists for a member variable, otherwise a shallow copy.
 *
 * There is a special C++ specification where automatic lvalues returned dby
 * value can be moved instead of copied. Any lvalue you initialize in a function
 * and return can be moved because that object will not exist after the
 * function.
 *
 * Rule of 5
 * If any of the copy copy constructor/assignment, move constructor/assingment,
 * or destructor is defined or deleted, then each of these should be defined or
 * deleted.
 * */

/*
 * std::move
 *
 * Casts its argument to an rvalue reference so that move semantics can be
 * invoked. We can make a lvalue into a rvalue reference so that move semantics
 * can be used.
 * */

template<typename T>
class Auto_ptr4
{
public:
	Auto_ptr4(T* ptr = nullptr)
		: _ptr { ptr }
	{
	}

	~Auto_ptr4()
	{
		delete _ptr;
	}

	// Copy constructor
	Auto_ptr4(const Auto_ptr4& a)
	{
		_ptr = new T;
		*_ptr = *a._ptr;
	}

	// Move constructor
	Auto_ptr4(Auto_ptr4&& a) noexcept
		: _ptr { a._ptr }
	{
		a._ptr = nullptr;
	}

	// Copy assignment
	Auto_ptr4& operator=(const Auto_ptr4& a)
	{
		if (&a == this)
			return *this;

		delete _ptr;

		_ptr = new T;
		*_ptr = *a._ptr;

		return *this;
	}

	// Move assignment
	Auto_ptr4& operator=(Auto_ptr4&& a) noexcept
	{
		if (&a == this)
			return *this;

		delete _ptr;

		_ptr = a._ptr;
		a._ptr = nullptr;

		return *this;
	}

	T& operator*() const { return *_ptr; }
	T* operator->() const { return _ptr; }
	bool isnull() const { return _ptr == nullptr; }

private:
	T* _ptr {};
};

class Resource
{
public:
	Resource() { std::cout << "Resource acquired\n"; }
	~Resource() { std::cout << "Resource destroyed\n"; }
};

Auto_ptr4<Resource> generateResource()
{
	Auto_ptr4<Resource> res{new Resource};
	return res; // this return value will invoke the move constructor
}

template<typename T>
void custom_swap(T&a, T&b) {
    T tmp {std::move(a)}; // instead of making a third copy, move by using std::move to get a rvalue refernece
    a = std::move(b);
    b = std::move(tmp);
}

int main()
{
    {
	Auto_ptr4<Resource> mainres;
	mainres = generateResource(); // this assignment will invoke the move assignment
    }

    std::string a {"a"};
    std::string b {"b"};
    cout << a << " " << b << "\n";
    custom_swap(a, b);
    cout << a << " " << b << "\n";

	return 0; 
}

