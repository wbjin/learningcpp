#include <mutex>
#include <utility>

// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.

namespace getcracked {
struct ControlBlock {
	size_t count_ {};
	mutable std::mutex mutex_;
};

template <typename T>
class SharedPointer {
public:
	SharedPointer() {}
	SharedPointer(T* pointer) : ptr_ {pointer} {
		try {
			cb_ = new ControlBlock {1};
		} catch (...) {
			delete ptr_;
			throw;
		}
	}

	SharedPointer(const SharedPointer& other) noexcept {
		/* Don't have to release because we don't own anything yet in ctor */
		copy_and_increment(other);
	}

	SharedPointer& operator=(const SharedPointer& other) noexcept {
		if (this == &other) return *this;

		SharedPointer tmp {other};
		std::swap(ptr_, tmp.ptr_);
		std::swap(cb_, tmp.cb_);
		return *this;
	}

	SharedPointer(SharedPointer&& other) noexcept { steal(std::move(other)); }

	SharedPointer& operator=(SharedPointer&& other) noexcept {
		if (this == &other) return *this;

		release();
		steal(std::move(other));
		return *this;
	}

	~SharedPointer() { release(); }

	void reset(T* pointer) {
		SharedPointer tmp {};

		if (pointer) tmp = SharedPointer(pointer);

		std::swap(ptr_, tmp.ptr_);
		std::swap(cb_, tmp.cb_);
	}

	size_t get_count() const {
		if (!cb_) return 0;

		std::scoped_lock lock {cb_->mutex_};
		return cb_->count_;
	}

	T* operator->() const { return ptr_; }
	T& operator*() const { return *ptr_; }
	explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
	void release() {
		T* old_ptr           = std::exchange(ptr_, nullptr);
		ControlBlock* old_cb = std::exchange(cb_, nullptr);

		if (!old_ptr) return;

		bool free = false;
		{
			std::scoped_lock lock {old_cb->mutex_};
			free = (--old_cb->count_ == 0);
		}

		if (free) {
			delete old_ptr;
			delete old_cb;
		}
	}

	void copy_and_increment(const SharedPointer& other) {
		ptr_ = other.ptr_;
		cb_  = other.cb_;
		if (!cb_) return;

		std::scoped_lock lock {cb_->mutex_};
		++cb_->count_;
	}

	void steal(SharedPointer&& other) {
		ptr_ = std::exchange(other.ptr_, nullptr);
		cb_  = std::exchange(other.cb_, nullptr);
	}

	T* ptr_ {nullptr};
	ControlBlock* cb_ {nullptr};
};
}  // namespace getcracked
