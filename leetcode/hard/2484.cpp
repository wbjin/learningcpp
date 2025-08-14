//dp,prefix,suffix
class Solution {
public:
    int countPalindromes(string s) {
        std::vector<std::vector<std::vector<int>>> pre (
            s.size(),
            std::vector<std::vector<int>>(10, std::vector<int>(10)));
        std::vector<std::vector<std::vector<int>>> suf (
            s.size(),
            std::vector<std::vector<int>>(10, std::vector<int>(10)));
        std::vector<int> digits (10, 0);
        
        for (int i {}; i < s.size(); ++i) {
            int val = s[i] - '0';
            if (i != 0) {
                for (int j = 0; j < 10; ++j) {
                    for (int k = 0; k < 10; ++k) {
                        pre[i][j][k] = pre[i-1][j][k];
                        if (val == k)
                            pre[i][j][k] += digits[j];
                    }
                }
            }
            digits[val]++;
        }

        digits = std::vector<int>(10, 0);
        for (int i {(int)s.size()-1}; i >= 0; --i) {
            int val = s[i] - '0';
            if (i != s.size()-1) {
                for (int j = 0; j < 10; ++j) {
                    for (int k = 0; k < 10; ++k) {
                        suf[i][j][k] = suf[i+1][j][k];
                        if (k == val)
                            suf[i][j][k] += digits[j];
                    }
                }
            }
            digits[val]++;
        }

        int ret {};
        int mod {1'000'000'000 + 7};
        for (int i {1}; i < s.size()-1; ++i) {
            int val {s[i] - '0'};
            for (int j {}; j < 10; ++j)
                for (int k {}; k < 10; ++k) {
                    int v = ((long long)pre[i-1][j][k] * (long long)suf[i+1][j][k])%mod;
                    ret = (ret + v) % mod;
                }
        }
        return ret;
    }
};

/*
 * Brute force of generating all subsequences and checking for palindrome and
 * length is too slow. Intuition is that for a string of length 5, palindrome is
 * XY_YX. For two digits there are 100 possibilities so you can store occurences
 * of each two digit number before and after a certain index of the string.
 * Multiple these together to get the count for that index.
 *
 * Note, the answer get's too big so when you multiply, you have to cast either
 * to a bigger integral type like long long
 * */
