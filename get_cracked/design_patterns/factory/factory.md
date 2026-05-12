# Factory Pattern

Compile with
```
g++ main.cpp -std=c++23 -o main
or
clang++ main.cpp -std=c++23 -o main
```

The factory attern is a way to decouple object creation from object usage. It
encapsulates creation in a separate class instead of directly calling the
constructor of the object. It is useful for when you have different sub classes
of the same base class interface that you may have to switch between in
runtime, when construction logic is complex, and you want to separate the
construction logic from the usage logic.

For example, we have a base `DBConnection` that is presumably constructed with
many parameters (though omitted here). There are many different databases and
thus different clients for these databases in the form of `MySQLConn` and
`PostgresConn`.

Each of these require a factory that implements the base interface
`DBConnectionFactory`. This factory is what implements the logic of managing N
number of connections at most. Note that the factory passes in a callback
function into the `DBConnection` that is created to actually keep the number of
connections up to date.

## C++ Notes
The rule of five/three/whatever number now says that if you define any of the
destructor, copy constructor/assignment, move constructor/assignment, you have
to define all of them. Note that in `DBConnection`, move and copies are deleted
for simplicity. Additionally, deleting the move and copies in the base
`DBConnection` class also enforces this in derived classes.
