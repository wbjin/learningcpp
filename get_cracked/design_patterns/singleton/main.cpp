#include <string>
#include <iostream>

class Logger {
public:
  static Logger& GetLogger() {
    static Logger singleton {"INFO"};
    return singleton;
  }

  std::string GetLevel() {
    return level_;
  }

  void SetLevel(std::string level) noexcept {
    level_ = level;
  }

  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  void operator=(const Logger&) = delete;
  void operator=(Logger&&) = delete;

private:
  Logger(std::string level) : level_(level) {}
  std::string level_{};
};

int main() {
  auto& logger = Logger::GetLogger();
  std::cout << logger.GetLevel() << "\n";

  logger.SetLevel("DEBUG");
  std::cout << Logger::GetLogger().GetLevel() << "\n";

  Logger::GetLogger().SetLevel("WARN");
  std::cout << Logger::GetLogger().GetLevel() << "\n";
}
