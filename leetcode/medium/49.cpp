//hashing,array
class Solution {
    struct Hasher {
        size_t operator()(const std::array<int, 26>& arr) const {
            size_t hash = 0;
            for (int val : arr) {
                hash = (hash*31+val);
            }
            return hash;
        }
    };
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        std::unordered_map<std::array<int, 26>, std::vector<std::string>, Solution::Hasher> groups;
        for (const auto& s : strs) {
            std::array<int, 26> counts {};
            for (auto c : s) {
                int index = c - 'a';
                ++counts[index];
            }
            groups[counts].push_back(s);
        }
        std::vector<std::vector<std::string>> out;
        out.reserve(groups.size());
        for (auto [_, value] : groups) {
            out.push_back(value);
        }
        return out;
    }
};

/*
 * Simple way is to sort the strings and use that as key to hash map. Better way
 * is to write hash function for the counts of each character. Use this as
 * reference for writing hash functions for custom hash keys.
 * */
