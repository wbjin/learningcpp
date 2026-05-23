#include <memory>
#include <cstdio>

class RouteStrategy {
public:
  virtual ~RouteStrategy() = default;
  virtual void Route() = 0;
};

class FastestRoute : public RouteStrategy {
public:
  void Route() override {
    printf("FastestRoute::Route()\n");
  }
};

class ShortestRoute : public RouteStrategy {
public:
  void Route() override {
    printf("ShortestRoute::Route()\n");
  }
};

class Navigator {
public:
	Navigator(std::unique_ptr<RouteStrategy> strategy) : strategy_ {std::move(strategy)} {}

  void Navigate() {
    strategy_->Route();
  }

private:
  std::unique_ptr<RouteStrategy> strategy_ {};
};

int main() {
  Navigator nav {std::make_unique<FastestRoute>()};
  nav.Navigate();

  nav = Navigator{std::make_unique<ShortestRoute>()};
  nav.Navigate();
}
