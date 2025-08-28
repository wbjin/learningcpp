//two pointer, dp
class Solution {
public:
    int countSubstrings(string s) {
        std::vector<std::vector<bool>> memo (s.size(), std::vector<bool>(s.size(), false));
        int count {};
        for (int i = s.size() - 1; i >= 0; --i) {
            for (int j {i}; j < s.size(); ++j) {
                if (i == j) {
                    memo[i][j] = true;
                }
                else if (s[i] == s[j] && j-i == 1) {
                    memo[i][j] = true;
                }
                else if (s[i] == s[j] && memo[i+1][j-1]) {
                    memo[i][j] = true;
                }

                if (memo[i][j])
                    ++count;
            }
        }
        return count;
    }
};
