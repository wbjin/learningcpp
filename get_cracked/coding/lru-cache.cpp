#include <cassert>
#include <iostream>
#include <list>
#include <unordered_map>

class LRUCache {
public:
	LRUCache(int capacity) : capacity_ {static_cast<std::size_t>(capacity)} {
		if (capacity < 1) throw std::invalid_argument("LRUCache capacity must be at least 1");
	}

	LRUCache(const LRUCache&)            = delete;
	LRUCache& operator=(const LRUCache&) = delete;
	LRUCache(LRUCache&&)                 = delete;
	LRUCache& operator=(LRUCache&&)      = delete;

	int get(int key) {
		if (!cache_.contains(key)) return -1;

		touch(key);
		return cache_.at(key).value;
	}

	void put(int key, int value) {
		if (cache_.contains(key)) {
			auto& e = cache_.at(key);
			e.value = value;
			touch(key);
			return;
		}

		if (cache_.size() == capacity_) {
			auto key_to_evict = queue_.front();
			cache_.erase(key_to_evict);
			queue_.pop_front();
		}

		Entry e {value};
		queue_.push_back(key);
		e.it = std::prev(queue_.end());
		cache_.insert({key, e});
	}

private:
	void touch(int key) {
		auto& e = cache_.at(key);
		queue_.splice(queue_.end(), queue_, e.it);
	}

	struct Entry {
		int value {};
		std::list<int>::iterator it {};
	};
	std::unordered_map<int, Entry> cache_ {};
	std::list<int> queue_ {};
	std::size_t capacity_ {};
};

int main() {
	// 1. Basic put/get
	{
		LRUCache cache(2);
		cache.put(1, 100);
		assert(cache.get(1) == 100);
	}

	// 2. Missing key returns -1
	{
		LRUCache cache(2);
		assert(cache.get(42) == -1);
		cache.put(1, 1);
		assert(cache.get(2) == -1);
	}

	// 3. Classic eviction sequence
	{
		LRUCache cache(2);
		cache.put(1, 1);
		cache.put(2, 2);
		assert(cache.get(1) == 1);  // 1 is now MRU
		cache.put(3, 3);            // evicts 2
		assert(cache.get(2) == -1);
		cache.put(4, 4);  // evicts 1
		assert(cache.get(1) == -1);
		assert(cache.get(3) == 3);
		assert(cache.get(4) == 4);
	}

	// 4. get() refreshes recency
	{
		LRUCache cache(3);
		cache.put(1, 1);
		cache.put(2, 2);
		cache.put(3, 3);
		cache.get(1);     // order (MRU->LRU): 1,3,2
		cache.get(2);     // order: 2,1,3
		cache.put(4, 4);  // evicts 3
		assert(cache.get(3) == -1);
		assert(cache.get(1) == 1);
		assert(cache.get(2) == 2);
		assert(cache.get(4) == 4);
	}

	// 5. Updating an existing key changes the value and refreshes recency
	{
		LRUCache cache(2);
		cache.put(1, 1);
		cache.put(2, 2);
		cache.put(1, 10);  // update, 1 is now MRU, no eviction
		assert(cache.get(1) == 10);
		cache.put(3, 3);  // evicts 2
		assert(cache.get(2) == -1);
		assert(cache.get(1) == 10);
		assert(cache.get(3) == 3);
	}

	// 6. Updating an existing key must not evict anything
	{
		LRUCache cache(2);
		cache.put(1, 1);
		cache.put(2, 2);
		cache.put(2, 20);
		cache.put(1, 11);
		assert(cache.get(1) == 11);
		assert(cache.get(2) == 20);
	}

	// 7. Capacity of 1
	{
		LRUCache cache(1);
		cache.put(1, 1);
		assert(cache.get(1) == 1);
		cache.put(2, 2);
		assert(cache.get(1) == -1);
		assert(cache.get(2) == 2);
		cache.put(2, 3);  // update in place
		assert(cache.get(2) == 3);
	}

	// 8. Value 0 and negative keys/values are valid and distinct from "missing"
	{
		LRUCache cache(2);
		cache.put(0, 0);
		cache.put(-1, -5);
		assert(cache.get(0) == 0);
		assert(cache.get(-1) == -5);
	}

	// 9. Larger sequence: only the most recent `capacity` keys survive
	{
		const int cap = 100;
		LRUCache cache(cap);
		for (int i = 0; i < 1000; ++i) cache.put(i, i * 2);
		for (int i = 0; i < 900; ++i) assert(cache.get(i) == -1);
		for (int i = 900; i < 1000; ++i) assert(cache.get(i) == i * 2);
	}

	// 10. Repeated access keeps a hot key alive through many insertions
	{
		LRUCache cache(3);
		cache.put(0, 0);
		for (int i = 1; i <= 50; ++i) {
			assert(cache.get(0) == 0);  // keep key 0 hot
			cache.put(i, i);
		}
		assert(cache.get(0) == 0);
		assert(cache.get(1) == -1);
	}

	// 11. A missed get must not affect recency or insert anything
	{
		LRUCache cache(2);
		cache.put(1, 1);
		cache.put(2, 2);
		assert(cache.get(99) == -1);  // miss, no side effects
		cache.put(3, 3);              // evicts 1 (LRU)
		assert(cache.get(1) == -1);
		assert(cache.get(2) == 2);
		assert(cache.get(3) == 3);
	}

	std::cout << "All tests passed\n";
	return 0;
}
