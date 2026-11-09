// Implement a fixed-capacity FIFO for a memory simulator.
//
// Each request has a byte address and a byte count. The queue stores requests
// by value. Its capacity is set at construction and may be zero.
//
// push(request): Append the request and return true. If the queue is full,
// return false and leave it unchanged.
// pop(): Remove and return the oldest request. If empty, return std::nullopt.
//
// First implement the queue with std::deque. Aim for O(1) push and pop.
// Then sketch a std::vector ring buffer with the same API. Explain how you
// would wrap indices, distinguish full from empty, and compare allocation
// behavior and memory locality.
//
// Be ready to explain which methods can be const and where you enforce the
// capacity and FIFO invariants.

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

struct MemoryRequest {
	std::uint64_t address;
	std::size_t byte_count;

	bool operator==(const MemoryRequest&) const = default;
};

class MemoryRequestQueue {
public:
	explicit MemoryRequestQueue(std::size_t capacity) : start_index_ {0}, end_index_ {0}, size_ {0} {
		buf_.resize(capacity);
	}
	MemoryRequestQueue(const MemoryRequestQueue&)            = delete;
	MemoryRequestQueue(MemoryRequestQueue&&)                 = delete;
	MemoryRequestQueue& operator=(const MemoryRequestQueue&) = delete;
	MemoryRequestQueue& operator=(MemoryRequestQueue&&)      = delete;

	// start_index_ tracks front
	// end_index_ tracks next index to populate (one past end)
	// size_ used to disambiguate empty and full
	bool push(MemoryRequest request) {
		if (is_full()) return false;
		buf_.at(end_index_) = request;
		end_index_          = (end_index_ + 1) % buf_.size();
		++size_;
		return true;
	}

	std::optional<MemoryRequest> pop() {
		if (size_ == 0) return std::nullopt;
		auto req     = buf_.at(start_index_);
		start_index_ = (start_index_ + 1) % buf_.size();
    --size_;
		return req;
	}

private:
  bool is_full() const {
    return size_ == buf_.size();
  }
  std::vector<MemoryRequest> buf_ {};
  std::size_t start_index_ {};
  std::size_t end_index_ {};
  std::size_t size_ {};
};

int main() {
	const MemoryRequest first {0x1000, 64};
	const MemoryRequest second {0x0040, 16};
	const MemoryRequest third {0x1000, 32};

	// An empty queue reports empty. It can accept a request afterward.
	{
		MemoryRequestQueue queue(2);
		assert(!queue.pop());
		assert(queue.push(first));
		assert(queue.pop() == first);
		assert(!queue.pop());
	}

	// A zero-capacity queue is always full and empty.
	{
		MemoryRequestQueue queue(0);
		assert(!queue.push(first));
		assert(!queue.pop());
	}

	// A failed push leaves the queued request intact.
	{
		MemoryRequestQueue queue(1);
		assert(queue.push(first));
		assert(!queue.push(second));
		assert(queue.pop() == first);
		assert(queue.push(second));
		assert(queue.pop() == second);
		assert(!queue.pop());
	}

	// Free one slot, refill it, then drain in arrival order. This sequence
	// also exercises wraparound when you try the ring-buffer version.
	{
		MemoryRequestQueue queue(2);
		assert(queue.push(first));
		assert(queue.push(second));
		assert(!queue.push(third));
		assert(queue.pop() == first);
		assert(queue.push(third));
		assert(queue.pop() == second);
		assert(queue.pop() == third);
		assert(!queue.pop());
	}

	// Changing a caller's request after push leaves the queued copy intact.
	{
		MemoryRequestQueue queue(1);
		auto request = first;
		assert(queue.push(request));
		request.byte_count = 1;
		assert(queue.pop() == first);
	}

	std::cout << "All tests passed\n";
}
