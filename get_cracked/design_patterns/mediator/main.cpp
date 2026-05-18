#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <format>

// Forward declare so that we can use it in our Mediator base interface
class Component;

class Mediator {
public:
  virtual ~Mediator() = default;
  virtual void Notify(const Component &compontent,
                      const std::string &event) = 0;
};

class Component {
protected:
  Mediator *dialog_ {};

public:
  ~Component() = default;
  
  void SetDialog(Mediator *mediator) {
    dialog_ = mediator;
  }
};

class LoginButton : public Component {
public:
  void Click() {
    if (!enabled_) {
      std::printf("LoginButton::ClicK() disabled\n");
      return;
    }

    dialog_->Notify(*this, "click");
  }

  bool enabled_ {};
};

class UsernameTextBox : public Component {
public:
  std::string username_ {};

  void SetText(std::string username) {
    username_ = username;
    dialog_->Notify(*this, "username");
  }
};

class PasswordTextBox : public Component {
public:
  std::string password_{};

  void SetPassword(std::string password) {
    password_ = password;
    dialog_->Notify(*this, "password");
  }
};

class LoginDialog : public Mediator {
public:
  LoginDialog(UsernameTextBox &username, PasswordTextBox &password,
              LoginButton &button)
      : username_{username}, password_{password}, button_{button} {
    username_.SetDialog(this);
    password_.SetDialog(this);
    button_.SetDialog(this);
  }

  void Notify(const Component&,
              const std::string &event) override {
    std::cout << std::format("LoginDialog::Notify({})\n", event);
    if (event == "username" || event == "password") {
      if (username_.username_.empty() || password_.password_.empty())
        return;
      button_.enabled_ = true;
      return;
    }

    std::cout << std::format("Logged in user {}\n", username_.username_);
  }

private:
  UsernameTextBox &username_;
  PasswordTextBox &password_;
  LoginButton &button_;
};

int main() {
  UsernameTextBox username {};
  PasswordTextBox password {};
  LoginButton login {};

  LoginDialog dialog {username, password, login};

  login.Click();

  username.SetText("abcd");
  login.Click();

  password.SetPassword("abcd");
  login.Click();
}
