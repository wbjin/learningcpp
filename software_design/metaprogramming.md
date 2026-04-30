# Metaprogramming

Metaprogramming is when a program treats itself as data.
- Instrospection: A program inspecting and reasoning about itself
- Intercession: After inspecting, behavior is changed
- Generation: After interspecting, new code or behavior is generated

```C++
template <typename T, int N>
struct Buffer {
    T data[N];
    static constexpr int length = N;
}
```
`N` is a non-type template parameter that is defined at compile time.
`constexpr` is used for `length` because this allows you to use `length` at
compile time. It is static because `length` doesn't have to exist in multiple
places for multiple instances of `Buffer<int, 10>`. Only one `length` is needed
for the same pair of `T` and `N`.

```C++
template<typename T>
auto get_size(const T &obj) -> decltype(obj.size()) {
    return obj.size();
}

template <typename T>
auto get_size(const T &obj) -> decltype(obj.length) {
    return obj.length;
}

auto get_size(...) -> int {
    throw std::runtime_error("no size method");
}
```
Overload resolution and Substitution Failure is Not An Error is used to
generate methods that allow the size of an object to be retrieved even if there
are multiple ways to retrieve the size `.size()` or `.length`. SFINAE in C++ is
done in the order of definition. First the compiler will check if `obj.size()`
is valid. If it is not, it moves onto the next one. `decltype(obj.size())` is
used to make the return type match the `.size()` methods return type. Next, the
compiler tries `obj.length `. If none works, it falls back to the catch-all
case with the `...` argument which matches anything.

```C++
int main() {
    std::vector<int> vec = {1, 2, 3};
    Buffer<int, 4> buf = {{1, 2, 3, 4}};

    std::cout << get_size(vec); // calls obj.size()
    std::cout << get_size(buf); // calls obj.length;
    std::Cout << get_size(1); // throws runtime error
}
```
