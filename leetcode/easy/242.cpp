//hashing,array
class Solution {
public:
    bool isAnagram(string s, string t) {
        std::array<int, 26> chars {};
        for (const char& c : s) {
            int index = c - 'a';
            chars[index]++;
        }
        for (const char& c : t) {
            int index = c - 'a';
            chars[index]--;
        }
        for (const int& count : chars) {
            if (count != 0) return false;
        }
        return true;
    }
};
