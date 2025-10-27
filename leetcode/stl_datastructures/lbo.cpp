#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
#include <list>

struct Order {
    std::string client {};
    double price {};
    int volume {};
    bool buy {}; // true is buy, false is sell

    Order(std::string client_, double price_, int volume_, bool buy_)
        : client(client_), price(price_), volume(volume_), buy(buy_) {}
};

struct OrderNode {
    std::list<Order>::iterator it {};
    double price {};
    bool buy {};

    OrderNode(auto it_, double price_, bool buy_)
        : it(it_), price(price_), buy(buy_) {}
};

// LBO for one symbol
class LimitOrderBook {
public:
    LimitOrderBook() = default;

    // returns an order id
    uint64_t PlaceOrder(double price, int volume, std::string client, bool buy);

    int CancelOrder(uint64_t orderid);

    int GetPriceVolume(double price, bool buy);

private:
    std::map<double, std::list<Order>, std::greater<double>> bids_ {};
    std::map<double, std::list<Order>> asks_ {};
    std::unordered_map<uint64_t, OrderNode> orders_ {};
    std::unordered_map<double, int> bidvolumes_ {};
    std::unordered_map<double, int> askvolumes_ {};
    uint64_t orderids_ {};

    std::optional<std::list<Order>::iterator> placebid_(double price, int volume, std::string client);
    std::optional<std::list<Order>::iterator> placeask_(double price, int volume, std::string client);
    void cancelorder_(std::list<Order>::iterator it, double price, bool buy);
};

uint64_t LimitOrderBook::PlaceOrder(double price, int volume, std::string client, bool buy) {
    std::optional<std::list<Order>::iterator> it = std::nullopt;
    if (buy) {
        it = placebid_(price, volume, client);
    } else {
        it = placeask_(price, volume, client);
    }
    if (it == std::nullopt) return orderids_++;
    orders_.emplace(orderids_, *it, price, buy);
    return orderids_++;
}

std::optional<std::list<Order>::iterator> LimitOrderBook::placebid_(double price, int volume, std::string client) {
    while (volume > 0 && !asks_.empty()) {
        auto bestorder = asks_.begin()->second.begin(); // first Order node in the doubly linked list
        int matched = std::min(volume, bestorder->volume);
        volume -= matched;
        bestorder->volume -= matched;
        std::cout << client << " bought " << matched << " @ " << bestorder->price << " from " << bestorder->client << "\n";
        if (bestorder->volume == 0)
            cancelorder_(bestorder, price, false);
    }

    if (volume) {
        auto list = bids_[price];
        list.emplace_back(client, price, volume, true);
        bidvolumes_[price] += volume;
        return list.end();
    }
    return std::nullopt;
}

std::optional<std::list<Order>::iterator> LimitOrderBook::placeask_(double price, int volume, std::string client) {
    while (volume > 0 && !bids_.empty()) {
        auto bestorder = bids_.begin()->second.begin(); // first Order node in the doubly linked list
        int matched = std::min(volume, bestorder->volume);
        volume -= matched;
        bestorder->volume -= matched;
        std::cout << client << " sold " << matched << " @ " << bestorder->price << " from " << bestorder->client << "\n";
        if (bestorder->volume == 0)
            cancelorder_(bestorder, price, true);
    }

    if (volume) {
        auto list =asks_[price];
        list.emplace_back(client, price, volume, false);
        askvolumes_[price] += volume;
        return list.end();
    }
    return std::nullopt;
}

int LimitOrderBook::CancelOrder(uint64_t orderid) {
    if (orders_.count(orderid) == 0)
        return -1;
    
    const OrderNode& node = orders_.at(orderid);
    cancelorder_(node.it, node.price, node.buy);
    return 0;
}

int LimitOrderBook::GetPriceVolume(double price, bool buy) {
    if (buy) {
        return bidvolumes_[price];
    }
    return askvolumes_[price];

}

void LimitOrderBook::cancelorder_(std::list<Order>::iterator it, double price, bool buy) {
    if (buy) {
        bidvolumes_[price] -= it->volume;
        bids_[price].erase(it);
    } else {
        askvolumes_[price] -= it->volume;
        asks_[price].erase(it);
    }
}

int main() {

}
