#include <array>
#include <chrono>
#include <cmath>
#include <print>
#include <vector>
#include <numeric>

using mono_clock = std::chrono::steady_clock;

auto measure_l1_latency(size_t num_measurements) {
  // Much larger than what an L1 cache is
  auto l1_cache_data = std::array<uint8_t, 1 * 1024 * 1024>{};
  std::vector<double> measurements(num_measurements, 0);
  size_t sink {};
  double eviction_time_ns {};

  auto start = mono_clock::now();

  for (size_t i {}; i < num_measurements; ++i) {
    auto start_eviction = mono_clock::now();
    for (size_t j {}; j < l1_cache_data.size(); ++j) {
      l1_cache_data[j] = 1;
      sink += l1_cache_data[j];
    }
		eviction_time_ns +=
		  std::chrono::duration_cast<std::chrono::nanoseconds>(mono_clock::now() - start_eviction)
		    .count();

		sink += l1_cache_data[0];
  }

  if (sink == 1)
    std::exit(1);

	auto total_ns =
	  std::chrono::duration_cast<std::chrono::nanoseconds>(mono_clock::now() - start).count();
	auto total_read_ns = total_ns - eviction_time_ns;
  auto avg_read_ns = total_read_ns / (double) num_measurements;
  std::println("Total time {} ns", total_ns);
  std::println("Average L1 read {} ns", avg_read_ns);

  return avg_read_ns;
}

int main() {
  std::println("Measuring L1 cache latency");

  size_t NUM_MEASUREMENTS {10000};
  auto total_start = mono_clock::now();
  auto avg_ns = measure_l1_latency(NUM_MEASUREMENTS);
	auto total_time =
	  std::chrono::duration_cast<std::chrono::milliseconds>(mono_clock::now() - total_start);

	std::println("Took {} measurements in time: {}", NUM_MEASUREMENTS, total_time);
}
