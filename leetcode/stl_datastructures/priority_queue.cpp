#include <iostream>
#include <queue>
#include <functional>

struct Data {
    int num {};

    Data(int num_in) : num{num_in} {}

    bool operator<(const Data& other) const {
        return num < other.num; // max heap
    }
};

struct DataComparator {
    bool operator() (const Data& l, const Data& r) {
        return l.num > r.num; // min heap
    }
};

void print_pqueue(std::priority_queue<int, std::vector<int>> pq) {
    if (pq.empty()) {
        std::cout << "[]\n";
        return;
    }
    std::cout << "[";
    while (pq.size() != 1) {
        std::cout << pq.top() << ", ";
        pq.pop();
    }
    std::cout << pq.top();
    std::cout << "]\n";
}

void print_pqueue(std::priority_queue<int, std::vector<int>, std::greater<int>> pq) {
    if (pq.empty()) {
        std::cout << "[]\n";
        return;
    }
    std::cout << "[";
    while (pq.size() != 1) {
        std::cout << pq.top() << ", ";
        pq.pop();
    }
    std::cout << pq.top();
    std::cout << "]\n";
}

void print_pqueue(std::priority_queue<Data> pq) {
    if (pq.empty()) {
        std::cout << "[]\n";
        return;
    }
    std::cout << "[";
    while (pq.size() != 1) {
        std::cout << pq.top().num << ", ";
        pq.pop();
    }
    std::cout << pq.top().num;
    std::cout << "]\n";
}

void print_pqueue(std::priority_queue<Data, std::vector<Data>, DataComparator> pq) {
    if (pq.empty()) {
        std::cout << "[]\n";
        return;
    }
    std::cout << "[";
    while (pq.size() != 1) {
        std::cout << pq.top().num << ", ";
        pq.pop();
    }
    std::cout << pq.top().num;
    std::cout << "]\n";
}

int main() {
    // default max heap
    std::priority_queue<int> pq;

    // insertion
    pq.push(0);
    pq.push(1);
    pq.push(2);
    print_pqueue(pq);

    // deletion
    pq.pop();
    print_pqueue(pq);

    // greater for min heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;
    minpq.push(0);
    minpq.push(1);
    minpq.push(2);
    print_pqueue(minpq);

    std::priority_queue<Data, std::vector<Data>> custommaxpq {};
    custommaxpq.emplace(5);
    custommaxpq.emplace(10);
    custommaxpq.emplace(15);
    print_pqueue(custommaxpq);

    std::priority_queue<Data, std::vector<Data>, DataComparator> customminpq {};
    customminpq.emplace(5);
    customminpq.emplace(10);
    customminpq.emplace(15);
    print_pqueue(customminpq);
}
