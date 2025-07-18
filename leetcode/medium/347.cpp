//hashing,heap
class Solution {
    struct Comparator {
        bool operator()(const std::pair<int, int>& x, const std::pair<int, int>& y) const {
            return x.first < y.first; // max heap
        }
    };
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        std::unordered_map<int, int> counts;
        for (auto n : nums) counts[n]++;
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, Comparator> max_freqs;
        for (auto [num, count] : counts) {
            max_freqs.push(std::pair<int, int>(count, num));
        }
        std::vector<int> out;
        while (k) {
            auto [count, num] = max_freqs.top();
            max_freqs.pop();
            out.push_back(num);
            k--;
        }
        return out;
    }
};

/*
 * You don't have to write a custom comparator, I think C++ priority queue
 * defaults to comparing the first element in a pair but for future reference.
 * Also, note that priroity_queue is max by default
 * */
