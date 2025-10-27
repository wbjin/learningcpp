#include <iostream>
#include <vector>
#include <ranges>

void print_vector(const std::vector<int>& vec) {
    std::cout << "[";
    for (auto i : vec | std::views::take(vec.size() - 1)) {
        std::cout << i << ", ";
    }
    std::cout << vec.back();
    std::cout << "]\n";
}

int main() {
    std::vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_vector(vec);

    // access
    vec[0] = 1;
    vec.at(1) = 0;
    print_vector(vec);

    // insertion
    vec.push_back(10);
    vec.insert(vec.begin() + 1, 1);
    print_vector(vec);

    // size
    std::cout << vec.size() << "\n";

    // deletion
    vec.pop_back();
    vec.erase(std::find(vec.begin(), vec.end(), 9)); // single iterator
    vec.erase(vec.begin() + 1, vec.begin() + 2); // range of iterators
    print_vector(vec);

    // assign a new vector to current vector
    vec.assign({0, 1, 2, 3, 4, 5});
    print_vector(vec);

    // reserve and print the capacity;
    vec.reserve(100);
    std::cout << vec.capacity() << "\n";

    // shrink to make capacity the same as size
    vec.shrink_to_fit();
    std::cout << vec.capacity() << "\n";

    // emplace to avoid copying
    std::vector<std::pair<int, int>> vecpair {};
    vecpair.emplace_back(1, 2);

    // reverse iteration
    for (auto it = vec.rbegin(); it != vec.rend(); ++it)
        *it;
}
