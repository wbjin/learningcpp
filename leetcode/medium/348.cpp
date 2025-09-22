class TicTacToe {
    struct Player {
        std::vector<int> rows_ {};
        std::vector<int> cols_ {};
        int diagonal_ {};
        int antidiag_ {};
        int n_ {};
        int playernum_ {};

        Player(int n, int playernum) : n_(n) , playernum_(playernum) {
            rows_.resize(n_);
            cols_.resize(n_);
        }

        int place(int row, int col) {
            rows_[row]++;
            cols_[col]++;
            if (row == col)
                diagonal_++;
                if (row + col == n_-1)
                    antidiag_++;
            else if (row + col == n_ -1)
                antidiag_++;
            
            if (rows_[row] == n_ || cols_[col] == n_ || diagonal_ == n_ || antidiag_ == n_)
                return playernum_;
            else
                return 0;
        }
    };
    
    Player player1;
    Player player2;

public:
    TicTacToe(int n) : player1(Player(n, 1)), player2(Player(n, 2)) {}
    
    int move(int row, int col, int player) {
        if (player == 1)
            return player1.place(row, col);
        else
            return player2.place(row, col);
    }
};

/**
 * Your TicTacToe object will be instantiated and called as such:
 * TicTacToe* obj = new TicTacToe(n);
 * int param_1 = obj->move(row,col,player);
 */
