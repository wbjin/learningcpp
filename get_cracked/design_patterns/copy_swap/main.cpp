#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>

class Example {
public:
	friend void swap(Example& lhs, Example& rhs) {
		std::swap(lhs.ptr_, rhs.ptr_);
		std::swap(lhs.size_, rhs.size_);
		std::swap(lhs.name_, rhs.name_);
	}

	Example()  = default;
	~Example() = default;

	Example(const Example& other) = default;
	Example(Example&& other)      = default;

	Example& operator=(const Example& other) {
		Example tmp(other);
		swap(*this, tmp);
		return *this;
	}

	Example& operator=(Example&& other) noexcept {
		Example tmp(std::move(other));
		swap(*this, tmp);
		return *this;
	}

private:
	int* ptr_ {};
	std::size_t size_ {};
	std::string name_ {};
};

int main() {}
