//hashing
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        std::unordered_map<int, int> match;
        match.reserve(nums.size());
        for (int i = 0; i < nums.size(); ++i) {
            if (match.count(nums[i])) {
                return {i, match[nums[i]]};
            }
            match[target-nums[i]] = i;
        }
        return {};
    }
};
