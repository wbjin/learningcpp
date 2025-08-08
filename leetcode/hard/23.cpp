//heap
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
    struct HeapElement {
        ListNode* node;
        int index;

        friend bool operator< (const HeapElement& lhs, const HeapElement& rhs) {
            return lhs.node->val > rhs.node->val;
        }
    };
    
public:
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        std::priority_queue<HeapElement, std::vector<HeapElement>> min_heap;
        int finished_sub_lists {};
        for (int i = 0; i < lists.size(); ++i) {
            if (lists[i] == nullptr) {
                ++finished_sub_lists;
            } else {
                min_heap.push(HeapElement{lists[i], i});
                lists[i] = lists[i]->next;
            }
        }

        ListNode dummy(0, nullptr);
        ListNode* curr_node {&dummy};

        while (finished_sub_lists != lists.size()) {
            HeapElement next_node { min_heap.top() };
            min_heap.pop();

            curr_node->next = next_node.node;
            curr_node = next_node.node;

            if (lists[next_node.index] == nullptr) {
                ++finished_sub_lists;
            } else {
                min_heap.push(HeapElement{lists[next_node.index], next_node.index});
                lists[next_node.index] = lists[next_node.index]->next;
            }
        }

        return dummy.next;
    }
};
