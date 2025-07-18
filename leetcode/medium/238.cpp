//array
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        std::vector<int> out(nums.size(), 1);
        out[0] = 1;
        for (int i = 0; i < nums.size()-1; ++i) {
            out[i+1] = nums[i] * out[i];
        }

        int n = 1;
        for (int i = nums.size()-1; i >= 0; --i) {
            out[i] = n * out[i];
            n *= nums[i];
        }
        return std::move(out);
    }
};

/*
 * Make two passes. One going left to right to find the product of all values to
 * the left of an index. Second pass right to left to find product of all
 * values to the right of the array. Combine the two to get result for that
 * index.
 *
 * */
