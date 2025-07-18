//hashing
class Solution {
private:
    int _get_box(int row, int col) {
        int box_row = row / 3;
        int box_col = col / 3;
        return box_row * 3 + box_col;
    }
public:
    bool isValidSudoku(vector<vector<char>>& board) {
        std::vector<std::unordered_set<char>> rows(board.size());
        std::vector<std::unordered_set<char>> cols(board.size());
        std::vector<std::unordered_set<char>> boxes(board.size());

        for (int row = 0; row < board.size(); ++row) {
            for (int col = 0; col < board[row].size(); ++col) {
                char entry = board[row][col];
                if (entry == '.') continue;

                if (rows[row].find(entry) != rows[row].end())
                    return false;
                rows[row].insert(entry);

                if (cols[col].find(entry) != cols[col].end())
                    return false;
                cols[col].insert(entry);

                int box = _get_box(row, col);
                if (boxes[box].find(entry) != boxes[box].end())
                    return false;
                boxes[box].insert(entry);
            }
        }
        return true;
    }
};
