//greedy
class Solution {
public:
    int matchPlayersAndTrainers(vector<int>& players, vector<int>& trainers) {
        std::sort(players.begin(), players.end(), std::less<>());
        std::sort(trainers.begin(), trainers.end(), std::less<>());

        int players_index = 0;
        int trainers_index = 0;
        int out = 0;
        for(; players_index < players.size() && trainers_index < trainers.size();) {
            if (players[players_index] <= trainers[trainers_index]) {
                ++out;
                ++players_index;
                ++trainers_index;
            } else {
                ++trainers_index;
            }
        }
        return out;
    }
};
