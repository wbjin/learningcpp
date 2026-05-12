#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

/*
 * This interface defines how to communicate with a database
 * */
class DBConnection {
public:
  explicit DBConnection(std::function<void()> on_disconnect)
      : on_disconnect_(std::move(on_disconnect)) {}

  virtual ~DBConnection() {
    std::cout << "~DBConnection(): Calling disconnect callback\n";
    on_disconnect_();
  }

  virtual std::string Query() = 0;

  // delete copy constructor and assignment
  DBConnection(const DBConnection &) = delete;
  DBConnection &operator=(const DBConnection &) = delete;

  // delete move constructor and assignment
  DBConnection(DBConnection &&) = delete;
  DBConnection &operator=(DBConnection &&) = delete;

private:
  std::function<void()> on_disconnect_;
};

/*
 * There are different client implementations for different databases
 * */
class MySQLConn : public DBConnection {
public:
  MySQLConn(std::function<void()> on_disconnect)
      : DBConnection(on_disconnect) {}

  std::string Query() override {
    std::cout << "MySQLConn::Query()\n";
    return "MySQL";
  }

  ~MySQLConn() override { std::cout << "~MySQLConn()\n"; }
};

class PostgresConn : public DBConnection {
public:
  PostgresConn(std::function<void()> on_disconnect)
      : DBConnection(on_disconnect) {}

  std::string Query() override {
    std::cout << "PostgresConn::Query()\n";
    return "MySQL";
  }

  ~PostgresConn() override { std::cout << "~PostgresConn()\n"; }
};

/*
 * A base factory interface defines a common way to create objects across
 * differnt factory implementations
 * */
class DBConnectionFactory {
public:
  virtual std::unique_ptr<DBConnection> CreateConn() = 0;

  virtual ~DBConnectionFactory() = default;
};

class MySQLConnFactory : public DBConnectionFactory {
public:
  std::unique_ptr<DBConnection> CreateConn() override {
    std::cout << "MySQLConnFactory::CreateConn(), num_connections_="
              << num_connections_ << "\n";
    if (num_connections_ >= MAX_CONNECTIONS_)
      return nullptr;
    ++num_connections_;
    return std::make_unique<MySQLConn>(disconnect_callback_);
  }

private:
  static constexpr int MAX_CONNECTIONS_{5};
  int num_connections_{};
  std::function<void()> disconnect_callback_ = [this] { --num_connections_; };
};

class PostgresConnFactory : public DBConnectionFactory {
public:
  std::unique_ptr<DBConnection> CreateConn() override {
    std::cout << "PostgresConnFactory::CreateConn(), num_connections_="
              << num_connections_ << "\n";
    if (num_connections_ >= MAX_CONNECTIONS_) {
      return nullptr;
    }
    ++num_connections_;
    return std::make_unique<PostgresConn>(disconnect_callback_);
  }

private:
  static constexpr int MAX_CONNECTIONS_{5};
  int num_connections_{};
  std::function<void()> disconnect_callback_ = [this] { --num_connections_; };
};

int main() {
  constexpr std::string_view DB_TYPE = "POSTGRES";
  std::unique_ptr<DBConnectionFactory> factory{};
  if (DB_TYPE == "POSTGRES") {
    factory = std::make_unique<PostgresConnFactory>();
  } else if (DB_TYPE == "MySQL") {
    factory = std::make_unique<MySQLConnFactory>();
  } else {
    throw std::invalid_argument("Unknown DB type");
  }

  std::vector<std::unique_ptr<DBConnection>> conns{};
  for (const auto &_ : std::views::iota(0, 6)) {
    auto conn = factory->CreateConn();
    if (conn)
      conns.push_back(std::move(conn));
    else
      std::cout << "Connection creation failed\n";
  }
}
