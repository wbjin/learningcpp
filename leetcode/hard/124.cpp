//recursion, tree
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
    struct Elm {
        TreeNode* node {};
        int curr_sum {};
    };

public:
    int maxPathSum(TreeNode* root) {
        int best {INT_MIN};
        helper(root, best);
        return best;
    }

    int helper(TreeNode* node, int& best) {
        if (!node) return 0;

        int left_sum {helper(node->left, best)};
        int right_sum {helper(node->right, best)};
        
        best = std::max(best, left_sum + node->val);
        best = std::max(best, right_sum + node->val);
        best = std::max(best, node->val);
        best = std::max(best, left_sum + right_sum + node->val);

        return std::max(left_sum + node->val, std::max(right_sum + node->val, node->val));
    }
};

/*
 * Keep track of two variables, the best sum seen so far and the best sum for
 * the local node. The best for local node is best sum between left + this,
 * right + this, or just this. Think of this is as taking the left path, right
 * path, or ending the path. For best sum seen globally, the choices are left
 * path, right path, just the node, or start new path.
 * */
