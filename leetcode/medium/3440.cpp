//greedy,array
class Solution {
public:
    int maxFreeTime(int eventTime, vector<int>& startTime, vector<int>& endTime) {
        std::vector<bool> non_adjacent_gaps(startTime.size(), false);
        int max_gap_left = 0;
        for (int i = 0; i < startTime.size(); ++i) {
            int slot_size = endTime[i] - startTime[i];
            if (slot_size <= max_gap_left)
                non_adjacent_gaps[i] = true;
            
            int prev_end_time = (i == 0) ? 0 : endTime[i-1];
            max_gap_left = std::max(max_gap_left, startTime[i]-prev_end_time);
        }

        int max_gap_right = 0;
        for (int i = startTime.size()-1; i >= 0; --i) {
            int slot_size = endTime[i] - startTime[i];
            if (slot_size <= max_gap_right)
                non_adjacent_gaps[i] = true;
            
            int prev_start_time = (i == startTime.size()-1) ? eventTime : startTime[i+1];
            max_gap_right = std::max(max_gap_right, prev_start_time-endTime[i]);
        }

        int out = 0;
        for (int i = 0; i < startTime.size(); ++i) {
            int left_end = (i == 0) ? 0 : endTime[i-1];
            int right_start = (i == startTime.size()-1) ? eventTime : startTime[i+1];

            int gap = right_start - left_end;
            if (!non_adjacent_gaps[i]) {
                gap -= (endTime[i] - startTime[i]);
                }
            out = std::max(out, gap);
        }
        return out;
    }
};

/*
 * For every slot, figure out whether you can move it a non adjacent gap. Do
 * this efficiently by doing two linear passes, one for finding things gaps to
 * the right, one for finding gaps to the left. If you can put in nonadjacent
 * gap, you can save slot size gap as well.
 * */
