#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>

class LFUCache {
public:
	explicit LFUCache(int capacity) : capacity_ {static_cast<std::size_t>(capacity)} {}

	LFUCache(const LFUCache&)            = delete;
	LFUCache& operator=(const LFUCache&) = delete;
	LFUCache(LFUCache&&)                 = delete;
	LFUCache& operator=(LFUCache&&)      = delete;

	int get(int key) {
		if (!cache_.contains(key)) return -1;
		auto val = cache_.at(key).value;
		touch(key);
		return val;
	}

	void put(int key, int value) {
		if (capacity_ == 0) return;
		if (cache_.contains(key)) {
			auto& entry = cache_.at(key);
			entry.value = value;
			touch(key);
			return;
		}

		while (cache_.size() == capacity_) {
			auto& [freq, lowest_freq_list] = *freqs_.begin();
			if (lowest_freq_list.empty()) {
				freqs_.erase(freq);
				continue;
			}

			auto key_to_evict = lowest_freq_list.front();
			cache_.erase(key_to_evict);
			lowest_freq_list.pop_front();
		}

		Entry e {value, 1};
		freqs_[1].push_back(key);
		e.it = std::prev(freqs_.at(1).end());
		cache_.insert({key, e});
	}

private:
	void touch(int key) {
		auto& e = cache_.at(key);
		freqs_.at(e.freq).erase(e.it);
		if (freqs_.at(e.freq).empty()) freqs_.erase(e.freq);

		e.freq++;
		freqs_[e.freq].push_back(key);
		e.it = std::prev(freqs_[e.freq].end());
	}

	struct Entry {
		int value {};
		int freq {};
		std::list<int>::iterator it {};
	};
	std::unordered_map<int, Entry> cache_ {};
	std::map<int, std::list<int>> freqs_ {};
	std::size_t capacity_ {};
};

void expectEqual(int actual, int expected, const std::string& label) {
	if (actual != expected) {
		std::cerr << "FAIL: " << label << " | expected " << expected << ", got " << actual << '\n';
		std::exit(EXIT_FAILURE);
	}
}

int main() {
	// 1. Empty cache and zero capacity.
	{
		LFUCache cache(0);
		cache.put(1, 1);
		expectEqual(cache.get(1), -1, "Capacity 0 should not allow any keys");
		cache.put(2, 1);
		expectEqual(cache.get(2), -1, "Capacity 0 should not allow any keys");
	}

	// 2. Capacity one, updates, and reinsertion.
	{
		LFUCache cache(1);
		cache.put(1, 10);
		cache.put(1, 11);
		expectEqual(cache.get(1), 11, "existing value updated");

		cache.put(2, 20);
		expectEqual(cache.get(1), -1, "capacity one: previous key evicted");
		expectEqual(cache.get(2), 20, "capacity one: replacement present");

		cache.put(1, 100);
		expectEqual(cache.get(2), -1, "capacity one: replacement evicted");
		expectEqual(cache.get(1), 100, "evicted key can be reinserted");
	}

	// 3. Standard LFU sequence, including a frequency tie.
	{
		LFUCache cache(2);
		cache.put(1, 1);
		cache.put(2, 2);
		expectEqual(cache.get(1), 1, "access raises key 1 frequency");

		cache.put(3, 3);  // Evicts key 2: frequency 1 versus key 1's 2.
		expectEqual(cache.get(2), -1, "least frequent key evicted");
		expectEqual(cache.get(3), 3, "key 3 present");

		// Keys 1 and 3 now both have frequency 2; key 1 is older.
		cache.put(4, 4);
		expectEqual(cache.get(1), -1, "LRU breaks frequency tie");
		expectEqual(cache.get(3), 3, "more recent tied key retained");
		expectEqual(cache.get(4), 4, "new key retained");
	}

	// 4. Insertion order breaks ties among never-accessed keys.
	{
		LFUCache cache(3);
		cache.put(1, 10);
		cache.put(2, 20);
		cache.put(3, 30);
		cache.put(4, 40);

		expectEqual(cache.get(1), -1, "oldest frequency-one key evicted");
		expectEqual(cache.get(2), 20, "second inserted key retained");
		expectEqual(cache.get(3), 30, "third inserted key retained");
		expectEqual(cache.get(4), 40, "fourth inserted key present");
	}

	// 5. Frequency takes priority over recency.
	{
		LFUCache cache(2);
		cache.put(1, 10);
		cache.put(2, 20);

		expectEqual(cache.get(1), 10, "key 1 reaches frequency 2");
		expectEqual(cache.get(1), 10, "key 1 reaches frequency 3");
		expectEqual(cache.get(2), 20, "key 2 is newer but less frequent");

		cache.put(3, 30);
		expectEqual(cache.get(2), -1, "newer but less frequent key evicted");
		expectEqual(cache.get(1), 10, "older high-frequency key retained");
	}

	// 6. Updating an existing key counts as an access.
	{
		LFUCache cache(2);
		cache.put(1, 10);
		cache.put(2, 20);
		cache.put(1, 100);  // Key 1 reaches frequency 2.
		cache.put(3, 30);

		expectEqual(cache.get(2), -1, "put update increases frequency");
		expectEqual(cache.get(1), 100, "updated value survives eviction");
		expectEqual(cache.get(3), 30, "insertion after update succeeds");
	}

	// 7. Updating with the same value still counts as an access.
	{
		LFUCache cache(2);
		cache.put(1, 10);
		cache.put(2, 20);
		cache.put(1, 10);
		cache.put(3, 30);

		expectEqual(cache.get(2), -1, "same-value put increases frequency");
		expectEqual(cache.get(1), 10, "same-value update retains key");
	}

	// 8. Recency at a higher frequency follows access order.
	{
		LFUCache cache(2);
		cache.put(1, 10);
		cache.put(2, 20);

		expectEqual(cache.get(2), 20, "key 2 enters frequency 2 first");
		expectEqual(cache.get(1), 10, "key 1 enters frequency 2 second");

		cache.put(3, 30);
		expectEqual(cache.get(2), -1, "tie uses access order, not insertion");
		expectEqual(cache.get(1), 10, "most recently accessed tie survives");
	}

	// 9. Reinserting an evicted key resets its frequency.
	{
		LFUCache cache(2);
		cache.put(1, 10);
		cache.put(2, 20);
		expectEqual(cache.get(1), 10, "key 1 frequency becomes 2");
		expectEqual(cache.get(2), 20, "key 2 frequency becomes 2");

		cache.put(3, 30);   // Evicts key 1.
		cache.put(1, 100);  // Evicts key 3; key 1 starts at frequency 1.
		cache.put(4, 40);   // Must evict key 1 again.

		expectEqual(cache.get(1), -1, "reinserted key starts fresh");
		expectEqual(cache.get(3), -1, "intermediate key was evicted");
		expectEqual(cache.get(2), 20, "resident frequency preserved");
		expectEqual(cache.get(4), 40, "latest key present");
	}

	// 10. Missing gets do not insert keys; zero is a valid key/value.
	{
		LFUCache cache(2);
		cache.put(0, 0);
		cache.put(1, 10);

		expectEqual(cache.get(99), -1, "missing get returns minus one");
		expectEqual(cache.get(99), -1, "repeated missing get has no effect");

		cache.put(2, 20);
		expectEqual(cache.get(0), -1, "misses preserve eviction order");
		expectEqual(cache.get(1), 10, "misses do not consume capacity");

		cache.put(2, 0);
		expectEqual(cache.get(2), 0, "zero value is valid");
	}

	std::cout << "All LFU cache tests passed.\n";
	return EXIT_SUCCESS;
}
