/*
 * LearnC++ 18.4 Timing your code
 *
 * copmile with make timer
 *
 * */

#include <array>
#include <numeric>
#include <optional>
#include <time.h>
#include <iostream>

using std::cout, std::endl;

class Timer {
public:
    Timer() {
        if (clock_gettime(CLOCK_MONOTONIC, &t) != 0)
            err = true;
    }

    std::optional<long> time() {
        struct timespec end {};
        if (clock_gettime(CLOCK_MONOTONIC, &end) != 0)
            return std::nullopt;
        
        if (err)
            return std::nullopt;

        long start_time_ns = t.tv_sec * 1'000'000'000 + t.tv_nsec;
        long end_time_ns = end.tv_sec * 1'000'000'000 + end.tv_nsec;

        return end_time_ns - start_time_ns;
    }

private:
    struct timespec t {};
    bool err {false};
};


int main() {
    std::array<int, 100000> arr {};
    std::iota(arr.rbegin(), arr.rend(), 1); // 100000, ..., 1

    Timer t {};

    std::sort(std::begin(arr), std::end(arr));

    auto time = t.time();
    if (time)
        cout << *time << " ns" << "\n";
}
