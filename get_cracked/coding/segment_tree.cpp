#include <memory>
#include <print>
#include <stdexcept>
#include <vector>

template <typename T, typename Combine = std::plus<T>>
class SegmentTree {
	struct Node {
		explicit Node(const T& value) : val {value} {}

		T val {};
		std::unique_ptr<Node> left {};
		std::unique_ptr<Node> right {};
	};

public:
	SegmentTree(int size, T identity, Combine combine = Combine())
	  : size_ {size},
	    identity_ {identity},
	    combine_(combine) {
		if (size_ <= 0) throw std::invalid_argument("Size must be positive");
	}

	SegmentTree(const std::vector<T>& values, T identity, Combine combine)
	  : SegmentTree(values.size(), identity, combine) {
		root_ = build(values, 0, size_ - 1);
	}

	void insert(int index, const T& value) {
		if (index < 0 || index >= size_) throw std::out_of_range("Invalid index");

		insert_(root_, 0, size_ - 1, index, value);
	}

	void update(int index, const T& value) { insert(index, value); }

	T query(int left, int right) const {;
		if (left < 0 || left > right || right >= size_) throw std::out_of_range("Invalid query range");

		return query_(root_, 0, size_ - 1, left, right);
	}

  void grow() {
    if (size_ > std::numeric_limits<int>::max() / 2) {
        throw std::length_error("Tree capacity is too large");
    }

    auto new_root = std::make_unique<Node>(identity_);

    // Existing readings occupy the left half.
    new_root->left = std::move(root_);
    new_root->val = combine_(
        valueOf(new_root->left),
        valueOf(new_root->right));

    root_ = std::move(new_root);
    size_ *= 2;
}

	int size() const { return size_; }

private:
	void insert_(std::unique_ptr<Node>& node, int low, int high, int index, const T& value) {
		if (!node) node = std::make_unique<Node>(identity_);

		if (low == high) {
			node->val = value;
			return;
		}

		const int mid = low + (high - low) / 2;
		if (index <= mid)
			insert_(node->left, low, mid, index, value);
		else
			insert_(node->right, mid + 1, high, index, value);

		node->val = combine_(valueOf(node->left), valueOf(node->right));
	}

	T query_(const std::unique_ptr<Node>& node, int low, int high, int left, int right) const {
		if (!node || right < low || high < left) return identity_;

		if (left <= low && high <= right) return node->val;

		const int mid = low + (high - low) / 2;
		return combine_(
		  query_(node->left, low, mid, left, right), query_(node->right, mid + 1, high, left, right)
		);
	}

	T valueOf(const std::unique_ptr<Node>& node) const { return node ? node->val : identity_; }

	std::unique_ptr<Node> build(const std::vector<T>& values, int low, int high) {
		if (low == high) return std::unique_ptr<Node>(values[low]);

		const int mid = low + (high - low) / 2;
		auto node     = std::make_unique<Node>(values[mid]);
		node->left    = build(values, low, mid);
		node->right   = build(values, mid + 1, high);
		node->value   = combine_(valueOf(node->left), valueOf(node->right));
	}

	int size_ {};
	T identity_ {};
	Combine combine_ {};
	std::unique_ptr<Node> root_ {};
};

int main() {
	{
		// Example of sum segment tree
		SegmentTree<int> tree {6, 0};
		tree.insert(0, 1);
		tree.insert(1, 3);
		tree.insert(2, 5);
    tree.insert(3, -8);
    tree.insert(4, -2);
    tree.insert(5, 8);

		std::println("Sum 0-2: {}", tree.query(0, 2));
    std::println("Sum 3-5 {}", tree.query(3, 5));
    std::println("Sum 1-4 {}", tree.query(1, 4));
	}

  {
    struct MaxCombine {
      int operator()(int a, int b) const {
        return std::max(a, b);
      }
    };

    SegmentTree<int, MaxCombine> tree {6, std::numeric_limits<int>::lowest()};
		tree.insert(0, 1);
		tree.insert(1, 3);
		tree.insert(2, 5);
    tree.insert(3, -8);
    tree.insert(4, -2);
    tree.insert(5, 8);

		std::println("Max 0-2: {}", tree.query(0, 2));
    std::println("Max 3-5 {}", tree.query(3, 5));
    std::println("Max 1-4 {}", tree.query(1, 4));
  }
}
