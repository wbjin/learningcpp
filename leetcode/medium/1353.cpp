//greedy,heap
class Solution {
public:
    int maxEvents(vector<vector<int>>& events) {
        auto comp = [](const std::vector<int>& a, const std::vector<int>& b) {
            return a[0] < b[0]; // ascending order
        };
        std::sort(events.begin(), events.end(), comp);

        int max_day = 0;
        for (const auto& event : events) {
            max_day = std::max(max_day, event[1]);
        }

        std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
        int ret = 0;
        int i = 0;
        for (int day = 0; day <= max_day; ++day) {
            while (i < events.size() && events[i][0] <= day) {
                min_heap.emplace(events[i][1]);
                i++;
            }
            while (!min_heap.empty() && min_heap.top() < day) {
                min_heap.pop();
            }
            if (!min_heap.empty()) {
                min_heap.pop();
                ret++;
            }
        }
        return ret;
    }
};

/*
 * Greedy approach by looking through the events in start time order
 * and allocating days for events that ends earliest using a min heap. Add
 * events that might overlap with current day by looking at start date. Pop
 * events where current day has passed end date.
 * */
