# PIMPL Pattern

The pointer to implementation, PIMPL, pattern is used to hide a class's
implementation details behind a pointer. This is done to reduce the compilation
time of a program. In C and C++, to use a pointer to an object defined in a
different header file, you don't need to include that header file. Instead, you
just have to forward declare that object. This is possible because the size of
a pointer is the same regardless of what it points to.

For example, the `Helper` in `helper.hpp` is a class that exposes an interface.
The underlying implementation of that interface is done in a separate class
defined in `helper.cpp` under the `HelperImpl` class. Note that `helper.hpp`
only contains a forward declaration of `class HelperImpl`. This means that
other files that include `helper.hpp` doesn't need to be compiled if the
implementation in `helper.cpp` changes.

To test this, we can do the following.
1. Compile each translation unit separately and create object files
```bash
$ g++ -std=c++20 helper.cpp -c -o helper.o
$ g++ -std=c++20 main.cpp -c -o main.o
```
2. Check the timestamps of these object files using `ls -l *.0`. Notice that
they are the same.
3. Link the object files together to produce the final binary. Run the binary
and observe the output
```bash
$ g++ main.o helper.o -o main
$ ./main
Helper::CallHelper
HelperImpl::CallHelperImpl
```
4. Now make a change in `HelperImpl` in `helper.cpp`.
5. Compile `helper.cpp` again, link the object files, and run the binary.
Notice that the output has changed.
```bash
$ g++ -std=c++20 helper.cpp -c -o helper.o
$ g++ main.o helper.o -o main
$ ./main
Helper::CallHelper
HelperImpl::CallHelperImpl
This was added later
```
6. Run `ls -l *.o` and see that only the time for the `helper.o` file has
changed.

This is the benefit of the PIMPL pattern. We don't have to change the header
file when changing the implementation because the core implementation is in the
cpp file and the data structures are isolated to that file.

The downside of PIMPL is that it causes a pointer indirection when using the
`Helper` class.
