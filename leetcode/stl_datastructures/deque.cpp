#include <iostream>
#include <deque>
#include <ranges>

void print_deque(const std::deque<int> deq) {
    std::cout << "[";
    for (auto i : deq | std::views::take(deq.size() - 1)) {
        std::cout << i << ", ";
    }
    std::cout << deq.back();
    std::cout << "]\n";
}

int main() {
    std::deque<int> deq = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_deque(deq);

    // random access
    deq[0] = 1;
    deq.at(1) = 0;
    print_deque(deq);

    // insertion deletion
    deq.push_back(10);
    deq.push_front(-1);
    print_deque(deq);

    deq.pop_back();
    deq.pop_front();
    print_deque(deq);

    // random insertion deletion
    deq.insert(deq.begin() + 3, 100);
    print_deque(deq);

    deq.erase(deq.begin() + 3);
    print_deque(deq);

    // erase-remove idiom
    deq.insert(deq.begin() + 5, 5, 100);
    print_deque(deq);

    deq.erase(std::remove(deq.begin(), deq.end(), 100), deq.end());
    print_deque(deq);
}
