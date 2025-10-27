#include <iostream>
#include <list>
#include <iterator>

void print_list(const std::list<int>& l) {
    if (l.empty()) {
        std::cout << "[]\n";
        return;
    }
    auto end = std::prev(l.end(), 1);
    std::cout << "[";
    for (auto it = l.begin(); it != end; ++it) {
        std::cout << *it << ", ";
    }
    std::cout << *end;
    std::cout << "]\n";
}

int main() {
    std::list<int> l = {0, 1, 2, 3, 5, 6, 7, 8, 9};
    print_list(l);

    // no random access, front and back access only
    l.front() = 9;
    l.back() = 0;
    print_list(l);

    // insertion deletion
    l.push_back(10);
    l.push_front(-1);
    print_list(l);

    l.pop_back();
    l.pop_front();
    print_list(l);

    // iterators
    auto it = l.begin();
    std::advance(it, 2);
    *it = 100;
    print_list(l);
    it = std::prev(it, 1);
    *it = 101;
    print_list(l);

    // random removal
    l.erase(it);
    print_list(l);

    // removal of values equals something
    l.remove(8);
    print_list(l);

    l.remove_if([](int x) {return x%2 == 1;});
    print_list(l);

    // in place sorting
    it = l.begin();
    std::advance(it, 2);
    l.insert(it, 4);
    l.insert(it, 8);;
    l.sort();
    print_list(l);

    // merge lists
    std::list<int> l2 = {11, 13, 15, 17};
    l.merge(l2);
    print_list(l);
    print_list(l2); // l2 now empty

    // move elements from one list to another
    l2 = {11, 13, 15, 17};
    it = l.begin();
    std::advance(it, 2);
    l.splice(it, l2); // move l2 into l at position 2
    print_list(l);

    // remove consecutive duplicates;
    l.sort();
    l.unique();
    print_list(l);
}
