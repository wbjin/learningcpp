//strings
class Solution {
public:
    string makeFancyString(string s) {
        return std::accumulate(s.begin(), s.end(), std::string{}, [](std::string acc, char c){
            std::size_t len = acc.size();
            if (len >= 2 && acc[len-1] == c && acc[len-2] == c )
                return acc;
            acc+=c;
            return acc;
        });
    }
};

/*
 * Accumulate takes in an accumulation value and optionally a binary operation
 * which is be default operator+ meaning by default it is addition. You can
 * define a custom lambda to adjust the concatenation logic.
 * */
