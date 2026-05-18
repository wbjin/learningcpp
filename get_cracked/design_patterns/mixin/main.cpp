#include <chrono>
#include <string>
#include <iostream>

class User {
public:
	User(std::string name) : name_ {name} {}

  void print() const {
    std::cout << "User{name_: " << name_ << "}\n";
  }

private:
	std::string name_ {};
};

class Order {
public:
  Order(int orderid) : orderid_{orderid} {}

  void print() const {
    std::cout << "Order{orderid_: " << orderid_ << "}\n";
  }

private:
  int orderid_ {};
};

template <typename T>
class Timestampped : public T {
public:
	explicit Timestampped(T base) : T(base), created_ {std::chrono::system_clock::now()} {}

	void printWithTimestamp() const {
    this->print();
    auto t = std::chrono::system_clock::to_time_t(created_);
    std::cout << "Created: " << std::ctime(&t);
  }

private:
  std::chrono::system_clock::time_point created_ {};
};

template <typename T>
class Serializable : public T {
public:
  template<typename... Args>
  Serializable(Args&&... args) : T(std::forward<Args>(args)...) {}

  std::string to_string() {
    return "{...}";
  }
};

int main() {
  User user {"abcd"};
  Order order {1};
  Timestampped<User> timestampped_user {user};
  Timestampped<Order> timestampped_order {order};

  timestampped_user.print();
  timestampped_order.print();

  timestampped_user.printWithTimestamp();
  timestampped_order.printWithTimestamp();

  Serializable<Timestampped<User>> serializable_timetampped_user {timestampped_user};
  serializable_timetampped_user.print();
  serializable_timetampped_user.printWithTimestamp();
  std::cout << serializable_timetampped_user.to_string() << "\n";
}
