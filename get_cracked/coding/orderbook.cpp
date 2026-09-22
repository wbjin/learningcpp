#include <cassert>
#include <cstddef>
#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

/*
 * Notes
 * - Initial implementation is std::map for bids and asks, each level contains a deque
 *
 * - std::map uses a balanced tree, typically red black tree to store prices in order. Other
 * alternatives are radix trees that compress nodes. In a radix tree, prices are represented in a
 * tree format where each node represents a bit in the price. Price levels that start with the same
 * bit prefix can be compressed into one node to reduce lookup time
 *
 * - Another optimization could be to use an array like structure to keep price levels sorted. You
 * would be capped on the range of prices that can be kept because you would have to allocate an
 * element per price level for bids and asks. You can index the vector with (price - min_price) /
 * tick_size. This allows you to iterate price levels contiguously, though the container per price
 * level may be something else.
 *
 * A BTree could also be a way to implement sorted price levels. A btree node keeps multiple sorted
 * keys and has a children per sorted key. This can help with keeping similar price levels near in
 * memory
 * */

using Id       = size_t;
using Price    = long;
using Quantity = int;

// Implement this.
class Order {
public:
	Order(Id orderId, Price level, bool isBuy, Quantity quantity)
	  : order_id_ {orderId},
	    level_ {level},
	    is_buy_ {isBuy},
	    quantity_ {quantity} {}

	Id OrderId() const noexcept { return order_id_; }
	Price Level() const noexcept { return level_; }
	bool IsBuy() const noexcept { return is_buy_; }
	Quantity OrderQuantity() const noexcept { return quantity_; }

	void Fill(Quantity size) {
		if (size > quantity_) throw std::logic_error("Fill size is greater than existing quantity");
		quantity_ -= size;
	}

private:
	Id order_id_ {};
	Price level_ {};
	bool is_buy_ {};
	Quantity quantity_ {};
};

using Orders = std::vector<Order>;

// DO NOT MODIFY.
struct Trade {
	Id OrderIdA;
	Id OrderIdB;  // Aggressor's OrderId
	Id AggressorOrderId;
	bool AggressorIsBuy;
	Price Level;
	Quantity Size;
};

using Trades = std::vector<Trade>;

class Orderbook {
public:
	// Implement AddOrder and CancelOrder.
	Trades AddOrder(const Order& order) {
		if (orders_.contains(order.OrderId())) return {};

		if (order.IsBuy())
			bids_[order.Level()].push_back(order);
		else
			asks_[order.Level()].push_back(order);

		orders_.insert({
		  order.OrderId(), {order.Level(), curr_priority_++, false}
		});

		return match();
	}

	void CancelOrder(Id orderId) {
		if (!orders_.contains(orderId)) return;

		orders_[orderId].canceled = true;
	}

private:
	Trades match() {
		Trades trades {};
		while (true) {
			if (bids_.empty() || asks_.empty()) break;

			auto& [bid_price, bids_at_price] = *bids_.begin();
			auto& [ask_price, asks_at_price] = *asks_.begin();
			if (bid_price < ask_price) break;

			while (!bids_at_price.empty() && !asks_at_price.empty()) {
				auto& bid = bids_at_price.front();
				auto& ask = asks_at_price.front();
				if (orders_[bid.OrderId()].canceled) {
					bids_at_price.pop_front();
					continue;
				}
				if (orders_[ask.OrderId()].canceled) {
					asks_at_price.pop_front();
					continue;
				}

				auto bid_priority = orders_[bid.OrderId()].priority;
				auto ask_priority = orders_[ask.OrderId()].priority;

				auto aggressor_id = bid_priority > ask_priority ? bid.OrderId() : ask.OrderId();
				auto existing_id  = bid_priority < ask_priority ? bid.OrderId() : ask.OrderId();
				auto fill_size    = std::min(bid.OrderQuantity(), ask.OrderQuantity());
				auto fill_price   = orders_[existing_id].level;

				ask.Fill(fill_size);
				bid.Fill(fill_size);
				trades.push_back(
				  Trade {
				    .OrderIdA         = existing_id,
				    .OrderIdB         = aggressor_id,
				    .AggressorOrderId = aggressor_id,
				    .AggressorIsBuy   = aggressor_id == bid.OrderId(),
				    .Level            = fill_price,
				    .Size             = fill_size
				  }
				);

				if (bid.OrderQuantity() == 0) {
					orders_.erase(bid.OrderId());
					bids_at_price.pop_front();
				}
				if (ask.OrderQuantity() == 0) {
					orders_.erase(ask.OrderId());
					asks_at_price.pop_front();
				}
			}

			if (bids_at_price.empty()) bids_.erase(bid_price);
			if (asks_at_price.empty()) asks_.erase(ask_price);
		}
		return trades;
	}

	struct OrderMetadata {
		Price level {};
		Id priority {};
		bool canceled {false};
	};

	std::map<Price, std::deque<Order>, std::greater<Price>> bids_ {};
	std::map<Price, std::deque<Order>> asks_ {};
	std::unordered_map<Id, OrderMetadata> orders_ {};
	static inline Id curr_priority_ {};
};
