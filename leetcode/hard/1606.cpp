//greedy, heap
class Solution {
public:
    vector<int> busiestServers(int k, vector<int>& arrival, vector<int>& load) {
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> end_times {};
        std::set<int> available {};
        std::unordered_map<int, int> serve_counts {};
        for (int i {}; i < k; ++i)
            available.insert(i);
        
        for (size_t i {}; i < arrival.size(); ++i) {
            int arrival_time {arrival[i]};
            while (!end_times.empty() && end_times.top().first <= arrival_time) {
                auto server {end_times.top()};
                end_times.pop();
                available.insert(server.second);
            }

            if (available.empty())
                continue;
            
            auto server = available.lower_bound(i % k);
            if (server == available.end())
                server = available.begin();

            serve_counts[*server]++;
            end_times.push({arrival_time + load[i], *server});
            available.erase(server);
        }

        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>> counts {};
        for (auto [server, count] : serve_counts) {
            counts.push({count, server});
        }

        std::vector<int> out {};
        auto most {counts.top()};
        counts.pop();
        out.push_back(most.second);
        while(!counts.empty() && most.first == counts.top().first) {
            out.push_back(counts.top().second);
            counts.pop();
        }  

        return out;
    }
};

/*
 * Intuition is that instead of searching every server for an available one, you
 * can keep an ordered set of servers by server number that are available. You
 * can keep update which servers are available before each request by keeping
 * heap of servers by min endtimes.
 * */
