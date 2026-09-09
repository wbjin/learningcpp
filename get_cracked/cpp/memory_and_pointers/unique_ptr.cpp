#include <stdexcept>
#include <utility>

namespace getcracked {
template <typename T>
struct custom_deleter {
	void operator()(T* pointer) const { delete pointer; }
};

template <typename T, typename custom_deleter = custom_deleter<T>>
class unique_ptr {
public:
	unique_ptr() : ptr_ {nullptr} {}
	unique_ptr(T* pointer) : ptr_ {pointer} {}

	unique_ptr(const unique_ptr&) { throw std::logic_error("Can't copy unique_ptr"); };
	unique_ptr& operator=(const unique_ptr&) { throw std::logic_error("Can't copy unique_ptr"); };

	unique_ptr(unique_ptr&& other) noexcept {
		ptr_ = other.release();
		/* A real move constructor would need to move over the deleter as well */
	}

	unique_ptr& operator=(unique_ptr&& other) noexcept {
		if (this == &other) return *this;

		reset(other.release());
		/* A real move operator would need to move over the deleter as well */
		return *this;
	}

	~unique_ptr() {
		custom_deleter deleter;
		deleter(ptr_);
	}

	T* release() { return std::exchange(ptr_, nullptr); }

	void reset(T* pointer) {
		custom_deleter deleter;
		deleter(ptr_);
		ptr_ = pointer;
		/* Reset is supposed to use the existing deleter */
	}

	bool is_owning() const { return ptr_ != nullptr; }

	T& operator*() const { return *ptr_; }
	T* operator->() const { return ptr_; }
	operator bool() const { return is_owning(); }

private:
	T* ptr_ {nullptr};
};
}  // namespace getcracked
