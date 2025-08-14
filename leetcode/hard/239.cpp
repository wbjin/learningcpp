//deque, monotonic
class Solution {
    
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        std::deque<int> dq {};
        std::vector<int> out (nums.size()-k+1, 0);
        for (size_t i {}; i < k-1; ++i) {
            while (!dq.empty() && nums[dq.back()] <= nums[i])
                dq.pop_back();
            dq.push_back(i);
        }

        for (size_t i {(size_t)k-1}; i < nums.size(); ++i) {
            while (!dq.empty() && dq.front() < i-k+1)
                dq.pop_front();

            while (!dq.empty() && nums[dq.back()] <= nums[i])
                dq.pop_back();
            dq.push_back(i);

            out[i-k+1] = nums[dq.front()];
        }
        return out;
    }
};

/*
 * Use a deque to keep track of the max value within the window. Front of deque
 * contains max value, at that window slice. Pop from back of deque if values
 * are smaller than current value, best value bubbles up to the front. Remove
 * values from front if out of index.
 * */
