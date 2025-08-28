//heap, greedy
class Solution {
public:
    int minMeetingRooms(vector<vector<int>>& intervals) {
        std::sort(intervals.begin(), intervals.end(),
            [](const std::vector<int>& l, const std::vector<int>& r) {
                return l[0] < r[0];
            }
        );
        std::priority_queue<int, std::vector<int>, std::greater<int>> heap {};
        heap.push(intervals[0][1]);
        for (int i {1}; i < intervals.size(); ++i) {
            auto end_time {heap.top()};
            heap.pop();
            if (intervals[i][0] < end_time) {
                // need new meeting room
                heap.push(end_time);
            }
            heap.push(intervals[i][1]);
        }
        return heap.size();
    }
};
