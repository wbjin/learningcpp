#include <iostream>
#include <set>

void print_set(const std::set<int>& s) {
    if (s.empty()) {
         std::cout << "[]\n";
         return;
    }
    std::cout << "[";
    auto end = std::prev(s.end(), 1);
    for (auto it = s.begin(); it != end; ++it) {
        std::cout << *it << ",";
    }
    std::cout << *end;
    std::cout << "]\n";
}

void print_set(const std::set<int, std::greater<int>>& s) {
    if (s.empty()) {
         std::cout << "[]\n";
         return;
    }
    std::cout << "[";
    auto end = std::prev(s.end(), 1);
    for (auto it = s.begin(); it != end; ++it) {
        std::cout << *it << ",";
    }
    std::cout << *end;
    std::cout << "]\n";
}

int main() {
    // by default ascending
    std::set<int> minset = {9, 0, 1, 2, 4, 5, 6, 7, 8};
    print_set(minset);

    // greater for descending
    std::set<int, std::greater<int>> maxset{minset.begin(), minset.end()};
    print_set(maxset);

    // insertion
    minset.insert(-1);
    minset.insert(100);
    print_set(minset);

    // deletion
    minset.erase(-1);
    minset.erase(100);
    print_set(minset);

    minset = {0, 0, 1, 3, 5, 6, 8};
    print_set(minset);

    //find
    auto it = minset.find(0);
    std::cout << *it << "\n";

    // first element >= N
    it = minset.lower_bound(2);
    std::cout << *it << "\n";

    // first element > N
    it = minset.upper_bound(3);
    std::cout << *it << "\n";

    // iterators to equal numbers
    auto pairange = minset.equal_range(0);
}
