# Singleton Pattern

The singleton pattern is a way to ensure within a program only one instance of
a class exists. This is done by making the constructor of the object private,
as shown with the `Logger` class. Instead, the class exposes a static getter
function, `GetLogger` that contains the logic for maintaing just a single
instance of the class.

The `static` keyword within a function ensures that an object is initialized
before any calls to the function and that the object persists throughout all
calls to that function in future calls. This means the constructor for the
`singleton` instance of the `Logger` class runs only once. `static` is also
thread safe. The getter method also returns the object by reference.

In C++, it is important the singleton class deletes move and copy constructor
and operators. Otherwise, the singleton will know longer be a singleton because
copies will occur.
