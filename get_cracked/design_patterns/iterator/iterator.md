# Iterator Pattern

The iterator pattern exposes an interface for traversing container while hiding
away the complexity of the code that implements the traversal. To work with
existing C++ `stl` code that works with iterators, your container just needs to
support `begin()`, `end()`, dereference (`*`), equalty, and increment operators
(`++i`, `i++`).

C++ containers often implement an iterator for that container as part of the
class for that container. For example, in our code `IntRange` has an `Iterator`
code as part of its class.

Iterators have something called iterator traits. Iterator traits define the
iterator to STL algorithms so that they can make optimizations.
- `iterator_category`: Defines the capabilities of an iterator such as whether
it supports bidrectional movement or jumping forward elements. Algorithms like
`std::advance` can take advantage of different iterator capabilities
    - `std::input_iterator_tag`: Read elements sequentially and move forward
    through each element once. If you increment the iterator, you cannot
    reference a past element even with a copy of the iterator before
    incrementing. Example: `std::cin`
    - `std::output_iterator_tag`: Write elements sequentially and move forward
    through each element once. Example `std::cout`
    - `std::forward_iterator_tag`: Can read and write and traverse forward
    multiple times
    - `std::bidirectional_iterator_tag`: Can move forward and backward
    - `std::random_access_iterator_tag`: Supports pointer like arithmetic like
    indexing to jump to any location (mimicing O(1) access)
    - `std::contiguous_iterator_tag`: Random access iterator where elements are
    guaranteed to be physically adjacent in memory
- `value_type`: The type of element that the iterator points to, ie if
`std::vector<std::string>` iterator, `std::string`
- `difference_type`: Type used for distances between iterators, ie the type of
`end() - begin()`. Typically `std::ptrdiff_t`
- `pointer`: The type of doing (`it.operator->()`) on an iterator
- `reference`: The type of doing `*it` on an iterator
