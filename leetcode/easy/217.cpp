//hashing
class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        std::unordered_set<int> seen;
        seen.reserve(nums.size());
        seen.insert(nums.begin(), nums.end());
        if (seen.size() != nums.size()) {
            return true;
        }
        return false;
    }
};
