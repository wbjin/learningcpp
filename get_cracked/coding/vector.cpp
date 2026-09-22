#include <sys/wait.h>
#include <cassert>
#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <limits>
#include <string>

// Nontrivial element for detecting missing construction/destruction.
struct Tracked {
  inline static int alive = 0;
  int value;

  explicit Tracked(int n) : value(n) { ++alive; }
  Tracked(const Tracked& other) : value(other.value) { ++alive; }
  Tracked(Tracked&& other) noexcept : value(other.value) { ++alive; }
  Tracked& operator=(const Tracked&) = delete;
  Tracked& operator=(Tracked&&) = delete;
  ~Tracked() { --alive; }
};

namespace custom {

template <typename Element>
class vector {
public:

  vector() {
    first = static_cast<Element*>(::operator new(sizeof(Element)));
    last = first;
    end = first + 1;
  }

  vector(const vector<Element>& other) {
    copy_from(other);
  }

	vector& operator=(const vector<Element>& other) {
		if (this == &other) return *this;
		if (!initialized()) return *this;

		copy_from(other);
		return *this;
	}

	vector(vector<Element>&& other) {
    steal_from(std::move(other));
  }

	vector& operator=(vector<Element>&& other) {
		if (this == &other) return *this;
		if (!initialized()) return *this;

		clear();
    ::operator delete(first);
		steal_from(std::move(other));
		return *this;
	}

	~vector() {
		if (!initialized()) return;
    clear();
    ::operator delete(first);
  }

	void push_back(const Element& element) {
    if (last == end) {
      Element copy {element};
      reserve(get_capacity() == 0 ?  1 : get_capacity() * 2);
      std::construct_at(last, std::move_if_noexcept(copy));
    } else {
      std::construct_at(last, element);
    }
    ++last;
	}

	const Element& at(std::size_t index) const {
		if (index < 0 || index >= get_size()) throw std::out_of_range("Index out of range");
		return first[index];
	}

	std::size_t get_size() const noexcept {
		if (!initialized()) return 0;
		return static_cast<size_t>(last - first);
	}

	std::size_t get_capacity() const noexcept {
		if (!initialized()) return 0;
		return static_cast<size_t>(end - first);
	}

  void clear() {
    while(get_size())
      pop_back();
  }

	void pop_back() {
    if (get_size() == 0) throw std::out_of_range("No elements added");
    --last;
    std::destroy_at(last);
	}

  void reserve(size_t new_capacity) {
    if (new_capacity <= get_capacity())
      return;

    auto size = get_size();
		auto new_first = static_cast<Element*>(::operator new(sizeof(Element) * new_capacity));
    auto new_last = new_first + size;
    auto new_end = new_first + new_capacity;

    try {
      // move or copy over existing elements
      for (std::size_t i {}; i < size; ++i) {
          std::construct_at(new_first + i, std::move_if_noexcept(first[i]));
      }

      for (std::size_t i {}; i < size; ++i) {
        std::destroy_at(first + i);
      }
    } catch(...) {
      // do something
      ::operator delete(new_first);
      throw;
    }

    std::swap(first, new_first);
    std::swap(last, new_last);
    std::swap(end, new_end);
    ::operator delete(new_first);
	}


private:
  void copy_from(const vector<Element>& other) {
		auto new_first = static_cast<Element*>(::operator new(sizeof(Element) * other.get_size()));
		auto new_last = new_first + other.get_size();
    auto new_end = new_first + other.get_size();

    try {
      for (std::size_t i {}; i < get_size(); ++i) {
        std::destroy_at(first+i);
      }

      for (std::size_t i {}; i < other.get_size(); ++i) {
        std::construct_at(new_first + i, other.at(i));
      }
    } catch(...) {
      ::operator delete(new_first);
      throw;
    }

    std::swap(first, new_first);
    std::swap(last, new_last);
    std::swap(end, new_end);
	}

  void steal_from(vector<Element>&& other) {
    first = std::exchange(other.first, nullptr);
    last = std::exchange(other.last, nullptr);
    end = std::exchange(other.end, nullptr);
  }

  bool initialized() const noexcept {
    return first;
  }

  Element* first {nullptr};
  Element* last {nullptr};
  Element* end {nullptr};
};
}  // namespace custom

int main() {
  // at() must reject invalid indices, including on an empty vector.
  {
    custom::vector<int> values;
    auto expect_out_of_range = [&](std::size_t index) {
      bool threw = false;
      try {
        (void)values.at(index);
      } catch (const std::out_of_range&) {
        threw = true;
      }
      assert(threw);
    };

    expect_out_of_range(0);
    values.push_back(7);
    expect_out_of_range(values.get_size());
    expect_out_of_range(std::numeric_limits<std::size_t>::max());
    assert(values.at(0) == 7);
  }

  // Growth must support nontrivial elements and aliased insertion.
  {
    custom::vector<std::string> values;
    const std::string original(200, 'x');
    values.push_back(original);
    while (values.get_size() < values.get_capacity()) {
      values.push_back(original);
    }

    const auto size = values.get_size();
    values.push_back(values.at(0));  // Forces reallocation.
    assert(values.get_size() == size + 1);
    for (std::size_t i = 0; i < values.get_size(); ++i) {
      assert(values.at(i) == original);
    }
  }

  // Copies own independent storage.
  {
    custom::vector<int> source;
    source.push_back(10);
    source.push_back(20);

    custom::vector<int> copy(source);
    copy.pop_back();
    copy.push_back(99);
    assert(source.at(1) == 20);
    assert(copy.at(1) == 99);

    custom::vector<int> assigned;
    assigned.push_back(-1);
    assigned = source;
    assigned.pop_back();
    assigned.push_back(77);
    assert(source.at(1) == 20);
    assert(assigned.at(1) == 77);
  }

  // Construction, reallocation, removal, and scope exit balance lifetimes.
  // Deleted assignment operators ensure growth uses construction.
  assert(Tracked::alive == 0);
  {
    Tracked seed(42);
    custom::vector<Tracked> values;
    for (int i = 0; i < 20; ++i) {
      values.push_back(seed);
      assert(Tracked::alive == static_cast<int>(values.get_size()) + 1);
    }
    values.pop_back();
    assert(values.get_size() == 19);
    assert(Tracked::alive == 20);
    for (std::size_t i = 0; i < values.get_size(); ++i) {
      assert(values.at(i).value == 42);
    }
  }
  assert(Tracked::alive == 0);

	custom::vector<int> v;

	// A new vector is empty.
	assert(v.get_size() == 0);
	assert(v.get_capacity() >= v.get_size());

	// Insert and access elements.
	v.push_back(10);
	v.push_back(20);
	v.push_back(30);

	assert(v.get_size() == 3);
	assert(v.at(0) == 10);
	assert(v.at(1) == 20);
	assert(v.at(2) == 30);
	assert(v.get_capacity() >= v.get_size());

	// Growth preserves existing elements.
	for (int i = 0; i < 100; ++i) { v.push_back(i); }

	assert(v.get_size() == 103);
	assert(v.at(0) == 10);
	assert(v.at(1) == 20);
	assert(v.at(2) == 30);
	for (int i = 0; i < 100; ++i) { assert(v.at(i + 3) == i); }
	assert(v.get_capacity() >= v.get_size());

	// Remove the last element without reducing capacity.
	const auto previous_capacity = v.get_capacity();
	v.pop_back();

	assert(v.get_size() == 102);
	assert(v.at(v.get_size() - 1) == 98);
	assert(v.get_capacity() == previous_capacity);

	// Remove all elements, then reuse the vector.
	while (v.get_size() > 0) { v.pop_back(); }
	assert(v.get_size() == 0);

	v.push_back(42);
	assert(v.get_size() == 1);
	assert(v.at(0) == 42);

  // Move construction transfers storage without copying elements.
  assert(Tracked::alive == 0);
  {
    Tracked seed(42);
    custom::vector<Tracked> source;
    source.push_back(seed);
    source.push_back(seed);
    const auto* original_storage = &source.at(0);

    {
      custom::vector<Tracked> moved(std::move(source));
      assert(moved.get_size() == 2);
      assert(&moved.at(0) == original_storage);
      assert(moved.at(0).value == 42);
      assert(moved.at(1).value == 42);
      assert(Tracked::alive == 3);

      // A moved-from vector must remain usable.
      source.clear();
      source.push_back(seed);
      assert(source.get_size() == 1);
      assert(source.at(0).value == 42);
      assert(&source.at(0) != &moved.at(0));
      assert(Tracked::alive == 4);
    }

    assert(Tracked::alive == 2);
    assert(source.at(0).value == 42);
  }
  assert(Tracked::alive == 0);

  // Move assignment releases the destination's existing elements.
  {
    Tracked seed(7);
    custom::vector<Tracked> destination;
    destination.push_back(seed);
    destination.push_back(seed);

    {
      custom::vector<Tracked> source;
      source.push_back(seed);
      const auto* original_storage = &source.at(0);
      assert(Tracked::alive == 4);

      auto& result = (destination = std::move(source));
      assert(&result == &destination);
      assert(destination.get_size() == 1);
      assert(&destination.at(0) == original_storage);
      assert(destination.at(0).value == 7);

      source.clear();
      assert(Tracked::alive == 2);
      source.push_back(seed);
      assert(source.get_size() == 1);
      assert(&source.at(0) != &destination.at(0));
      assert(Tracked::alive == 3);
    }

    // Destroying the source must not invalidate the destination.
    assert(Tracked::alive == 2);
    assert(destination.at(0).value == 7);
  }
  assert(Tracked::alive == 0);

  // Moving empty vectors still leaves usable destinations.
  {
    custom::vector<int> empty;
    custom::vector<int> moved(std::move(empty));
    assert(moved.get_size() == 0);
    moved.push_back(10);
    assert(moved.at(0) == 10);

    custom::vector<int> another_empty;
    moved = std::move(another_empty);
    assert(moved.get_size() == 0);
    moved.push_back(20);
    assert(moved.at(0) == 20);
  }
}
