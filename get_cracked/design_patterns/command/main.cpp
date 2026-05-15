#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>

/*
 * THe base interface for a command object. Concrete commands inherit from this
 * base class
 * */
class Command {
public:
  virtual ~Command() = default;
  virtual void Execute() = 0;
  virtual void Undo() = 0;
};

/*
 * The "receiver" or the class that implements the underlying action.
 * */
class Account {
public:
  Account(const std::string &name, int balance)
      : name_{name}, balance_{balance} {}

  bool Debit(int amount) {
    std::printf("Account::Debit(%d)\n", amount);
    if (amount > balance_)
      return false;
    balance_ -= amount;
    return true;
  }

  void Credit(int amount) {
    std::printf("Account::Credit(%d)\n", amount);
    balance_ += amount;
  }

  int GetBalance() { return balance_; }

private:
  std::string name_ {};
  int balance_ {};
};

/*
 * Concrete implementations of a command.
 * */
class DebitCommand : public Command {
public:
  DebitCommand(Account &account, int amount)
      : account_{account}, amount_{amount} {};

  void Execute() override {
    if (!account_.Debit(amount_))
      throw std::runtime_error("Insufficient funds");
    executed_ = true;
  }

  void Undo() override {
    if (!executed_)
      return;
    account_.Credit(amount_);
  }

private:
  Account &account_;
  int amount_ {};
  bool executed_ {};
};

class CreditCommand : public Command {
public:
  CreditCommand(Account &account, int amount)
      : account_{account}, amount_{amount} {};

  void Execute() override {
    account_.Credit(amount_);
    executed_ = true;
  }

  void Undo() override {
    if (!executed_)
      return;
    if (!account_.Debit(amount_))
      throw std::runtime_error("Insufficient funds");
  }

private:
  Account &account_;
  int amount_ {};
  bool executed_ {};
};

/*
 * The "invoker" that calls Execute on the commands
 * */
class AccountManager {
public:
  void Execute(std::unique_ptr<Command> cmd) {
    cmd->Execute();
    history_.push_back(std::move(cmd));
  }

  void Rollback() {
    if (history_.empty()) return;
    history_.back()->Undo();
    history_.pop_back();
  }

private:
  std::vector<std::unique_ptr<Command>> history_ {};
};

int main() {
  AccountManager manager {};
  Account account{"A", 1000};
  std::printf("Account::GetBalance(): %d\n", account.GetBalance());

  manager.Execute(
    std::make_unique<DebitCommand>(account, 250)
  );
  std::printf("Account::GetBalance(): %d\n", account.GetBalance());

  manager.Execute(
    std::make_unique<CreditCommand>(account, 350)
  );
  std::printf("Account::GetBalance(): %d\n", account.GetBalance());

  manager.Rollback();
  std::printf("Account::GetBalance(): %d\n", account.GetBalance());

  manager.Rollback();
  std::printf("Account::GetBalance(): %d\n", account.GetBalance());
}
