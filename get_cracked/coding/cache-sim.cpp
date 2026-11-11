#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <span>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <utility>

template <std::size_t NumSets, std::size_t NumWays, std::size_t LineSize>
class Cache {
public:
	static_assert(std::has_single_bit(LineSize));
	static_assert(std::has_single_bit(NumSets));
	using Address = std::uint64_t;
	using Word    = std::array<std::byte, 4>;

	struct Stats {
		std::uint64_t hits {};
		std::uint64_t capacity_misses {};
		std::uint64_t compulsory_misses {};
		std::uint64_t conflict_misses {};

		std::uint64_t TotalMisses() { return capacity_misses + compulsory_misses + conflict_misses; }
	};

	// The caller owns the bytes and keeps them alive for the cache's lifetime.
	Cache(std::span<std::byte> memory) : backing_store_ {memory} {
		if (backing_store_.size() < NumSets * NumWays * LineSize)
			throw std::invalid_argument("Backing store is not big enough");
	};
	Cache(const Cache&)            = delete;
	Cache(Cache&&)                 = delete;
	Cache& operator=(const Cache&) = delete;
	Cache& operator=(Cache&&)      = delete;

	Word Load(Address address) {
		Word word {};
		std::size_t filled {};
		while (filled < word.size()) {
			Address current_start    = address + filled;
			LineOffset line_offset   = get_line_offset(current_start);
			Address cache_line_start = get_line_start_address(current_start);
			std::size_t to_copy      = std::min(word.size() - filled, LineSize - line_offset);

			Line& line = get_or_fill_line(cache_line_start);
			std::copy_n(line.data.begin() + line_offset, to_copy, word.begin() + filled);
			filled += to_copy;
		}
		return word;
	};

	void Store(Address address, Word data) {
		std::size_t filled {};
		while (filled < data.size()) {
			Address current_start    = address + filled;
			LineOffset line_offset   = get_line_offset(current_start);
			Address cache_line_start = get_line_start_address(current_start);
			std::size_t to_copy      = std::min(data.size() - filled, LineSize - line_offset);

			Line& line = get_or_fill_line(cache_line_start);
			std::copy_n(data.begin() + filled, to_copy, line.data.begin() + line_offset);
			line.dirty = true;
			filled += to_copy;
		}
	};

	Stats GetStats() const { return stats_; };

private:
	using Tag        = std::size_t;
	using LineOffset = std::size_t;
	using SetOffset  = std::size_t;

	struct Line {
		std::array<std::byte, LineSize> data {};
		Address start_address {};
		Tag tag {};
		std::uint64_t last_used {};
		bool dirty {false};
		bool occupied {};
	};

	struct Set {
		std::array<Line, NumWays> ways;
		std::size_t num_occupied {};
		std::uint64_t access_count {};

		std::optional<std::reference_wrapper<Line>> get_line(Tag tag) {
			for (auto& l : ways) {
				if (l.occupied && l.tag == tag) {
					l.last_used = ++access_count;
					return std::ref(l);
				}
			}
			return std::nullopt;
		}

		Line& insert(Address addr, const std::array<std::byte, LineSize>& line) {
			assert(!full());
			std::size_t open_index {SIZE_T_MAX};
			for (std::size_t i {}; i < ways.size(); ++i) {
				if (!ways[i].occupied) {
					open_index = i;
					break;
				}
			}
			ways[open_index].data          = line;
			ways[open_index].start_address = addr;
			ways[open_index].tag           = get_tag(addr);
			ways[open_index].last_used     = ++access_count;
			ways[open_index].dirty         = false;
			ways[open_index].occupied      = true;
			num_occupied++;
			return ways[open_index];
		}

		Line evict() {
			assert(full());
			std::size_t evict_index {0};
			std::uint64_t oldest_last_used = ways[0].last_used;
			for (std::size_t i {}; i < ways.size(); ++i) {
				if (ways[i].last_used < oldest_last_used) {
					evict_index      = i;
					oldest_last_used = ways[i].last_used;
				}
			}
			ways[evict_index].occupied = false;
			num_occupied--;
			return ways[evict_index];
		}

		std::size_t full() const { return num_occupied == NumWays; }
	};

	Line& get_or_fill_line(Address addr) {
		SetOffset set_offset = get_set_offset(addr);
		Tag tag              = get_tag(addr);
		Set& set             = cache_.at(set_offset);
		auto line_opt        = set.get_line(tag);
		bool hit             = line_opt ? true : false;
		record_access(addr, hit);

		if (line_opt) return line_opt->get();

		if (set.full()) {
			auto line = set.evict();
			if (line.dirty) write_line_to_backing(line.start_address, line.data);
		}
		Address line_addr = get_line_start_address(addr);
		auto line         = read_line_from_backing(line_addr);
		return set.insert(line_addr, line);
	}

	static LineOffset get_line_offset(Address addr) { return addr & kLineOffsetMask; }

	static SetOffset get_set_offset(Address addr) {
		return (addr >> kSetOffsetBits) & kSetOffsetMask;
	}

	static Tag get_tag(Address addr) { return addr >> kTagOffsetBits; }

	static Address get_line_start_address(Address addr) { return addr & ~kLineOffsetMask; }

	std::array<std::byte, LineSize> read_line_from_backing(Address address) {
		auto subspan = backing_store_.subspan(address, LineSize);
		std::array<std::byte, LineSize> line;
		std::ranges::copy(subspan, line.begin());
		return line;
	}

	void write_line_to_backing(Address address, std::array<std::byte, LineSize>& data) {
		assert(get_line_start_address(address) == address);
		auto dest = backing_store_.subspan(address, LineSize);
		std::ranges::copy(data, dest.begin());
	}

	Word read_word(const std::array<std::byte, LineSize>& line, std::size_t offset) {
		assert(offset + sizeof(Word) < LineSize);
		Word w {};
		std::copy_n(line.begin() + offset, sizeof(Word), w.begin());
		return w;
	}

	std::array<Set, NumSets> cache_ {};
	std::span<std::byte> backing_store_ {};
	Stats stats_ {};

	// Separate shadow structure is needed to track the type of miss
	void record_access(Address addr, bool hit) {
		std::size_t line_number = addr / LineSize;
		auto prev_exists        = last_accessed_.contains(line_number);

		if (hit) {
			++stats_.hits;
		} else if (!prev_exists) {
			++stats_.compulsory_misses;
		} else {
			auto last_accessed = last_accessed_.at(line_number);
			std::size_t new_lines_after {};
			for (const auto& [line, time] : last_accessed_) {
				if (time > last_accessed) new_lines_after++;
			}

			if (new_lines_after >= kLineCapacity)
				++stats_.capacity_misses;
			else
				++stats_.conflict_misses;
		}
		last_accessed_[line_number] = access_count_++;
	}
	std::unordered_map<std::size_t, std::uint64_t> last_accessed_ {};
	std::uint64_t access_count_ {};
	static constexpr std::size_t kLineCapacity = NumSets * NumWays;
	static constexpr Address kLineOffsetMask   = LineSize - 1;
	static constexpr Address kSetOffsetMask    = NumSets - 1;
	static constexpr Address kSetOffsetBits    = std::countr_zero(LineSize);
	static constexpr Address kTagOffsetBits    = kSetOffsetBits + std::countr_zero(NumSets);
};

// Test contract:
// - Byte addresses; set = (address / LineSize) % NumSets.
// - LRU replacement within each set; both loads and stores refresh recency.
// - Write allocate. Either write-through or write-back is supported here:
//   modified data must survive eviction, but immediate RAM updates aren't required.
// - Each load/store counts once. First-ever access to a line is compulsory;
//   a later miss is capacity if an equal-sized fully associative LRU shadow
//   cache also misses, otherwise conflict. The shadow sees every access.
// All accesses below are valid, word-aligned, and contained within one line.
// Run once Load, Store, and GetStats are implemented:
// clang++ -std=c++20 -Wall -Wextra -pedantic cache-sim.cpp -o /tmp/cache-sim-tests
// /tmp/cache-sim-tests
namespace {
using Memory = std::array<std::byte, 4096>;
using Word   = std::array<std::byte, 4>;

Memory MakeMemory() {
	Memory memory {};
	for (std::size_t i = 0; i < memory.size(); ++i)
		memory[i] = static_cast<std::byte>((i * 37 + i / 256) % 256);
	return memory;
}

Word ReadWord(const Memory& memory, std::size_t address) {
	return {
	  memory.at(address), memory.at(address + 1), memory.at(address + 2), memory.at(address + 3)
	};
}

void Expect(bool condition, std::string_view scenario, std::size_t step, std::string_view detail) {
	if (!condition) {
		std::cerr << "FAIL: " << scenario << " | step " << step << " | " << detail << '\n';
		std::exit(EXIT_FAILURE);
	}
}

enum class Outcome { Hit, Compulsory, Conflict, Capacity };
struct Access {
	std::size_t address;
	Outcome outcome;
};

template <typename Stats>
void ExpectStats(Stats actual, Stats expected, std::string_view name, std::size_t step) {
	Expect(actual.hits == expected.hits, name, step, "hit count");
	Expect(
	  actual.compulsory_misses == expected.compulsory_misses, name, step, "compulsory miss count"
	);
	Expect(actual.conflict_misses == expected.conflict_misses, name, step, "conflict miss count");
	Expect(actual.capacity_misses == expected.capacity_misses, name, step, "capacity miss count");
	Expect(actual.TotalMisses() == expected.TotalMisses(), name, step, "total miss count");
}

template <std::size_t Sets, std::size_t Ways, std::size_t Bytes = 16>
void LoadTrace(std::string_view name, std::initializer_list<Access> accesses) {
	auto memory = MakeMemory();
	Cache<Sets, Ways, Bytes> cache(memory);
	typename Cache<Sets, Ways, Bytes>::Stats expected {};
	ExpectStats(cache.GetStats(), expected, name, 0);
	std::size_t step = 0;
	for (auto [address, outcome] : accesses) {
		++step;
		Expect(cache.Load(address) == ReadWord(memory, address), name, step, "loaded word bytes");
		switch (outcome) {
			case Outcome::Hit: ++expected.hits; break;
			case Outcome::Compulsory: ++expected.compulsory_misses; break;
			case Outcome::Conflict: ++expected.conflict_misses; break;
			case Outcome::Capacity: ++expected.capacity_misses; break;
		}
		ExpectStats(cache.GetStats(), expected, name, step);
		// Reading stats must not count as a cache access or clear the counters.
		ExpectStats(cache.GetStats(), expected, name, step);
	}
}

template <std::size_t Sets, std::size_t Ways, std::size_t Bytes>
void StoreTests(std::string_view name) {
	using TestCache     = Cache<Sets, Ways, Bytes>;
	auto memory         = MakeMemory();
	const auto original = memory;
	TestCache cache(memory);
	const Word first {std::byte {0xDE}, std::byte {0xAD}, std::byte {0xBE}, std::byte {0xEF}};
	const Word second {std::byte {0}, std::byte {0xFF}, std::byte {0x80}, std::byte {1}};

	cache.Store(4, first);  // Cold store allocates the whole line.
	ExpectStats(cache.GetStats(), typename TestCache::Stats {0, 0, 1, 0}, name, 1);
	Expect(cache.Load(4) == first, name, 2, "read after cold store");
	Expect(cache.Load(0) == ReadWord(original, 0), name, 3, "store preserves preceding word");
	Expect(cache.Load(8) == ReadWord(original, 8), name, 4, "store preserves following word");
	cache.Store(4, second);  // Store hit overwrites all four bytes.
	Expect(cache.Load(4) == second, name, 6, "read after store hit");
	ExpectStats(cache.GetStats(), typename TestCache::Stats {5, 0, 1, 0}, name, 6);

	// Fill the same set with Ways other lines to force the modified line out.
	for (std::size_t i = 1; i <= Ways; ++i) {
		const auto address = i * Sets * Bytes;
		Expect(cache.Load(address) == ReadWord(original, address), name, 6 + i, "eviction load");
	}
	Expect(ReadWord(memory, 4) == second, name, 7 + Ways, "evicted store persisted to RAM");
	Expect(cache.Load(4) == second, name, 8 + Ways, "stored word survives reload");
	Expect(
	  cache.Load(0) == ReadWord(original, 0), name, 9 + Ways, "eviction preserves neighboring data"
	);
	ExpectStats(
	  cache.GetStats(),
	  typename TestCache::Stats {6, Sets == 1 ? 1u : 0u, Ways + 1, Sets == 1 ? 0u : 1u}, name,
	  9 + Ways
	);
}
}  // namespace

int main() {
	using enum Outcome;

	// Direct mapped: four sets, one way, 16-byte lines.
	LoadTrace<4, 1>(
	  "direct: cold line, spatial locality, repeat hits",
	  {
	    {0,  Compulsory},
      {0,  Hit       },
      {4,  Hit       },
      {8,  Hit       },
      {12, Hit       },
      {16, Compulsory}
	}
	);
	LoadTrace<4, 1>(
	  "direct: independent sets retain all four lines", {
	                                                      {0,  Compulsory},
	                                                      {16, Compulsory},
	                                                      {32, Compulsory},
	                                                      {48, Compulsory},
	                                                      {48, Hit       },
	                                                      {32, Hit       },
	                                                      {16, Hit       },
	                                                      {0,  Hit       }
	}
	);
	LoadTrace<4, 1>(
	  "direct: same-set conflict despite spare capacity",
	  {
	    {0,  Compulsory},
      {16, Compulsory},
      {64, Compulsory},
      {16, Hit       },
      {0,  Conflict  },
      {64, Conflict  }
	}
	);
	LoadTrace<4, 1>(
	  "direct: working set exceeds total capacity", {
	                                                  {0,  Compulsory},
	                                                  {16, Compulsory},
	                                                  {32, Compulsory},
	                                                  {48, Compulsory},
	                                                  {64, Compulsory},
	                                                  {0,  Capacity  }
	}
	);
	LoadTrace<4, 1, 4>(
	  "direct: one word per line",
	  {
	    {0,  Compulsory},
      {4,  Compulsory},
      {0,  Hit       },
      {16, Compulsory},
      {0,  Conflict  }
	}
	);
	StoreTests<4, 1, 16>("direct: store allocation, update, eviction");

	// Two-way set associative: two sets, four lines total.
	LoadTrace<2, 2>(
	  "two-way: colliding lines coexist",
	  {
	    {0,  Compulsory},
      {32, Compulsory},
      {0,  Hit       },
      {32, Hit       },
      {36, Hit       }
	}
	);
	LoadTrace<2, 2>(
	  "two-way: hit refreshes LRU, other set survives", {
	                                                      {0,  Compulsory},
	                                                      {32, Compulsory},
	                                                      {16, Compulsory},
	                                                      {0,  Hit       },
	                                                      {64, Compulsory},
	                                                      {0,  Hit       },
	                                                      {16, Hit       },
	                                                      {32, Conflict  }
	}
	);
	LoadTrace<2, 2>(
	  "two-way: alternating sets use full capacity", {
	                                                   {0,  Compulsory},
	                                                   {16, Compulsory},
	                                                   {32, Compulsory},
	                                                   {48, Compulsory},
	                                                   {0,  Hit       },
	                                                   {16, Hit       },
	                                                   {32, Hit       },
	                                                   {48, Hit       }
	}
	);
	LoadTrace<2, 2>(
	  "two-way: capacity miss after five distinct lines", {
	                                                        {0,  Compulsory},
	                                                        {16, Compulsory},
	                                                        {32, Compulsory},
	                                                        {48, Compulsory},
	                                                        {64, Compulsory},
	                                                        {0,  Capacity  }
	}
	);
	LoadTrace<2, 4, 32>(
	  "four-way: larger lines and fifth colliding block", {
	                                                        {0,   Compulsory},
	                                                        {28,  Hit       },
	                                                        {64,  Compulsory},
	                                                        {128, Compulsory},
	                                                        {192, Compulsory},
	                                                        {0,   Hit       },
	                                                        {256, Compulsory},
	                                                        {64,  Conflict  }
	}
	);
	StoreTests<2, 2, 16>("two-way: store allocation, update, eviction");
	StoreTests<2, 4, 32>("four-way: store allocation, update, eviction");

	// Fully associative: one set, four ways. No conflict misses are expected.
	LoadTrace<1, 4>(
	  "fully associative: arbitrary tags share all ways", {
	                                                        {0,   Compulsory},
	                                                        {64,  Compulsory},
	                                                        {128, Compulsory},
	                                                        {192, Compulsory},
	                                                        {0,   Hit       },
	                                                        {64,  Hit       },
	                                                        {128, Hit       },
	                                                        {192, Hit       }
	}
	);
	LoadTrace<1, 4>(
	  "fully associative: LRU eviction follows hit recency", {
	                                                           {0,  Compulsory},
	                                                           {16, Compulsory},
	                                                           {32, Compulsory},
	                                                           {48, Compulsory},
	                                                           {0,  Hit       },
	                                                           {64, Compulsory},
	                                                           {0,  Hit       },
	                                                           {16, Capacity  }
	}
	);
	LoadTrace<1, 4>(
	  "fully associative: cyclic working set thrashes", {
	                                                      {0,  Compulsory},
	                                                      {16, Compulsory},
	                                                      {32, Compulsory},
	                                                      {48, Compulsory},
	                                                      {64, Compulsory},
	                                                      {0,  Capacity  },
	                                                      {16, Capacity  },
	                                                      {32, Capacity  },
	                                                      {48, Capacity  },
	                                                      {64, Capacity  }
	}
	);
	LoadTrace<1, 4, 32>(
	  "fully associative: final word of backing memory",
	  {
	    {4092, Compulsory},
      {4064, Hit       },
      {4088, Hit       },
      {4032, Compulsory},
      {4092, Hit       }
	}
	);
	LoadTrace<1, 1>(
	  "single line: replacement and spatial locality",
	  {
	    {0,  Compulsory},
      {12, Hit       },
      {16, Compulsory},
      {0,  Capacity  },
      {4,  Hit       }
	}
	);
	StoreTests<1, 4, 16>("fully associative: store allocation, update, eviction");
	StoreTests<1, 1, 16>("single line: store allocation, update, eviction");

	std::cout << "All cache simulator tests passed\n";
}
