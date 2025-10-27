#include <iostream>
#include <unordered_set>

void print_set(const std::unordered_multiset<int>& s) {
    if (s.empty()) {
         std::cout << "[]\n";
         return;
    }
    std::cout << "[";
    auto end = s.end();
    for (auto it = s.begin(); it != end; ++it) {
        std::cout << *it << ",";
    }
    std::cout << "]\n";
}

struct CaseInsensitiveHash {
    size_t operator()(const std::string& s) const {
        std::string lower;
        lower.reserve(s.size());
        for (char c : s) lower.push_back(std::tolower(c));
        return std::hash<std::string>()(lower);
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& a, const std::string& b) const {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); i++)
            if (std::tolower(a[i]) != std::tolower(b[i])) return false;
        return true;
    }
};

int main() {
    std::unordered_multiset<int> s {0, 1, 1, 2, 2, 2, 3, 4};
    print_set(s);

    // count number of elements for each bucket
    std::cout << s.count(1) << "\n";
    std::cout << s.bucket_size(s.bucket(2)) << "\n";

    // remove all 2s
    s.erase(2);

    // custom comparator
    std::unordered_multiset<std::string, CaseInsensitiveHash, CaseInsensitiveEqual> customset {};
    customset.insert("Apple");
    customset.insert("apple");
    customset.insert("APPLE");
    customset.insert("AppLe");
    std::cout << customset.count("apple") << "\n";
}
