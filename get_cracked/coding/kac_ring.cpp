#include <cassert>
#include <cmath>
#include <vector>

class KacRing {
public:
	KacRing(int N, const std::vector<int>& marked) {
		period_ = N * 2;
		saved_.resize(period_);
		std::vector<bool> is_marked(N, false);
		for (auto i : marked) is_marked[i] = true;

		int total = N;
		std::vector<int> balls(N, 1);
		std::vector<int> next(N);
		for (std::size_t time {}; time < saved_.size(); ++time) {
			saved_[time] = static_cast<double>(total) / N;

			int next_total {};
			for (std::size_t i {}; i < N; ++i) {
				int color = balls[i];
				if (is_marked[i]) color = -color;

				next_total += color;
				next[(i + 1) % N] = color;
			}
			balls = next;
			total = next_total;
		}
		curr_step_ = 0;
	}

	void step() { curr_step_ = (curr_step_ + 1) % period_; }

	void kstep(long long k) { curr_step_ = (curr_step_ + k) % period_; }

	double color() { return saved_[curr_step_]; }

private:
	std::vector<double> saved_ {};
	std::size_t curr_step_ {};
	std::size_t period_ {};
};

int main() {
	auto check = [](double actual, double expected) { assert(std::abs(actual - expected) < 1e-9); };

	// No marked positions.
	{
		KacRing ring(5, {});
		check(ring.color(), 1.0);
		ring.step();
		ring.kstep(1'000'000'000'000'000'000LL);
		check(ring.color(), 1.0);
	}

	// Single-position ring: flips every step.
	{
		KacRing ring(1, {0});
		check(ring.color(), 1.0);
		ring.step();
		check(ring.color(), -1.0);
		ring.kstep(0);
		check(ring.color(), -1.0);
		ring.kstep(1'000'000'000'000'000'000LL);
		check(ring.color(), -1.0);
		ring.step();
		check(ring.color(), 1.0);
	}

	// One mark: all colors invert after one lap.
	{
		KacRing ring(4, {0});
		check(ring.color(), 1.0);
		ring.step();
		check(ring.color(), 0.5);
		ring.kstep(3);
		check(ring.color(), -1.0);
		ring.kstep(4);
		check(ring.color(), 1.0);

		ring.kstep(1'000'000'000'000'000'003LL);
		check(ring.color(), -0.5);  // Equivalent to 3 steps.
	}

	// Every position marked: all balls flip each step.
	{
		KacRing ring(4, {0, 1, 2, 3});
		ring.step();
		check(ring.color(), -1.0);
		ring.step();
		check(ring.color(), 1.0);
	}

	// Exhaustive small-ring tests against a direct simulation.
	for (int n = 1; n <= 7; ++n) {
		for (int mask = 0; mask < (1 << n); ++mask) {
			std::vector<int> marked;
			for (int i = 0; i < n; ++i) {
				if (mask & (1 << i)) marked.push_back(i);
			}

			KacRing ring(n, marked);
			std::vector<int> balls(n, 1);  // White = +1, black = -1.

			auto simulate = [&](int steps) {
				while (steps--) {
					std::vector<int> next(n);
					for (int i = 0; i < n; ++i) {
						next[(i + 1) % n] = (mask & (1 << i)) ? -balls[i] : balls[i];
					}
					balls.swap(next);
				}
			};

			auto verify = [&]() {
				int total = 0;
				for (int ball : balls) total += ball;
				check(ring.color(), static_cast<double>(total) / n);
				check(ring.color(), static_cast<double>(total) / n);
			};

			verify();
			for (int t = 0; t < 4 * n; ++t) {
				ring.step();
				simulate(1);
				verify();
			}

			for (int k : {0, 1, 2, n, n + 1, 2 * n + 3, 100}) {
				ring.kstep(k);
				simulate(k);
				verify();
			}
		}
	}

	return 0;
}
