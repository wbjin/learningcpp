//greedy,stack
class Solution {
private:
    std::pair<std::string, int> remove_substring(const std::string& str, std::string substring) {
        int num_removed {};
        std::vector<char> stack;
        stack.reserve(str.size());

        for (auto c : str) {
            if (c == substring[1]) {
                char peek = !stack.empty() ? stack.back() : '?';
                if (peek == substring[0]) {
                    num_removed+=1;
                    stack.pop_back();
                } else {
                    stack.push_back(c);
                }
            } else {
                stack.push_back(c);
            }
        }

        return {std::string(stack.begin(), stack.end()), num_removed};
    }

public:
    int maximumGain(string s, int x, int y) {
        std::vector<char> stack {};
        stack.reserve(s.size());

        if (x > y) {
            auto removed1 = remove_substring(s, "ab");
            auto removed2 = remove_substring(removed1.first, "ba");
            return removed1.second * x + removed2.second * y;
        } 

        auto removed1 = remove_substring(s, "ba");
        auto removed2 = remove_substring(removed1.first, "ab");
        return removed1.second * y + removed2.second * x;
    }
};
