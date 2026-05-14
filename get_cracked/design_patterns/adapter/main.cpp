#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*
 * Differnet systems with different interfaces that we want a unified interface
 * for
 * */
class LoggerA {
public:
  void Info(const std::string& m) {
    std::cout << "[LoggerA::Info] " << m << "\n";
  }
};

class LoggerB {
public:
  void LogMessage(const char *msg) {
    printf("[LoggerB::LogMessage] %s\n", msg);
  }
};

/*
 * We define a common interface that adapters must follow
 * */
class LoggerAdapter {
public:
  virtual ~LoggerAdapter() = default;
  virtual void Log(const std::string&) = 0;
};

class LoggerAAdapter : public LoggerAdapter {
public:
  void Log(const std::string& m) override {
    logger_.Info(m);
  }

private:
  LoggerA logger_ {};
};

/*
 * The adapter contains some logic to translate between the interfaces
 * */
class LoggerBAdapter : public LoggerAdapter {
public:
  void Log(const std::string& m) override {
    logger_.LogMessage(m.c_str());
  }

private:
  LoggerB logger_ {};
};

int main() {
  std::vector<std::unique_ptr<LoggerAdapter>> loggers {};
  loggers.emplace_back(std::make_unique<LoggerAAdapter>());
  loggers.emplace_back(std::make_unique<LoggerBAdapter>());

  std::string message = "Message from main";
  for (const auto& logger : loggers) {
    logger->Log(message);
  }
}
