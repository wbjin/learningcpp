#include <numeric>
#include <cstdio>

class IntRange {
public:
  IntRange(int start, int end) : start_{start}, end_{end} {}

  class Iterator {
  public:
    // Iterator traits describe the iterator to the STL algorithms
    
    // Only forward iteration, each element only one time
    using iterator_category = std::input_iterator_tag;
    // The type of element the iterator conceptually points to
    using value_type = int;
    // The type of difference between two iterators
    using difference_type = std::ptrdiff_t;
    // The result type of i->()
    using pointer = const int *;
    // The tresult of doing *i. Not actually a reference for this becasue we
    // return temp copies for dereferences
    using reference = int;

    explicit Iterator(int value) : curr_{value} {};

    int operator*() {
      return curr_;
    }

    // prefix increment operator
    Iterator& operator++() {
      ++curr_;
      return *this;
    }

    Iterator operator++(int) {
      Iterator old = *this;
      ++(*this);
      return old;
    }

    bool operator==(const Iterator& other) const {
      return curr_ == other.curr_;
    }

    // C++ 20 can figure this out just from operator==
    // bool operator!=(const Iterator &other) {
    //   return !(*this == other);
    // }

  private:
    int curr_ {};
  };

  Iterator begin() const {
    return Iterator(start_);
  }

  Iterator end() const {
    return Iterator(end_ + 1);
  }

private:
  int start_ {};
  int end_ {};
};

int main() {
  // inclusive range
  IntRange range {2, 15};
  for (auto it = range.begin(); it != range.end(); ++it) {
    std::printf("%d, ", *it);
  }
  printf("\n");

  int sum = std::accumulate(range.begin(), range.end(), 0);
  std::printf("Sum: %d\n", sum);
}
