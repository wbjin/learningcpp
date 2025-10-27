#include <iostream>
#include <map>

void print_map(const std::map<int, int>& s) {
    if (s.empty()) {
         std::cout << "{}\n";
         return;
    }
    std::cout << "{";
    auto end = std::prev(s.end(), 1);
    for (auto it = s.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second << ", ";
    }
    std::cout << end->first << ": " << end->second;
    std::cout << "}\n";
}

void print_map(const std::map<int, int, std::greater<int>>& s) {
    if (s.empty()) {
         std::cout << "{}\n";
         return;
    }
    std::cout << "{";
    auto end = std::prev(s.end(), 1);
    for (auto it = s.begin(); it != end; ++it) {
        std::cout << it->first << ": " << it->second << ", ";
    }
    std::cout << end->first << ": " << end->second;
    std::cout << "}\n";
}

int main() {
    // by default ascending
    std::map<int, int> minmap {{0, 0}, {1, 1}, {2, 2}};
    print_map(minmap);

    // change to descending
    std::map<int, int, std::greater<int>> maxmap {minmap.begin(), minmap.end()};
    print_map(maxmap);

    minmap[3] = 3;
    minmap.at(0) = 1;
    print_map(minmap);

    // find
    auto it = minmap.find(3);

    minmap.insert({10, 10});
    print_map(minmap);
    // find first key >= N
    it = minmap.lower_bound(3);
    std::cout << it->first << "\n";

    it = minmap.upper_bound(3);
    std::cout << it->first << "\n";
}
