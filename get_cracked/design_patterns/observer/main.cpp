#include <cstdio>
#include <memory>
#include <vector>

class Observer {
public:
  virtual ~Observer() = default;
  virtual void Update(int temp) = 0;
};

class WeatherStation {
public:
  void Subscribe(std::unique_ptr<Observer> observer) {
    observers_.push_back(std::move(observer));
  }

  void SetTemp(int temp) {
    temp_ = temp;
    for (auto& obs : observers_) {
      obs->Update(temp_);
    }
  }

private:
  std::vector<std::unique_ptr<Observer>> observers_ {};
  int temp_ {};
};

class PhoneDisplay : public Observer {
public:
  void Update(int temp) override {
    std::printf("PhoneDisplay::Update(%d)\n", temp);
  }
};

class WindowDisplay : public Observer {
public:
  void Update(int temp) override {
    std::printf("WindowDisplay::Update(%d)\n", temp);
  }
};

int main() {
  WeatherStation station {};
  
  station.SetTemp(50);

  station.Subscribe(std::make_unique<PhoneDisplay>());
  station.Subscribe(std::make_unique<WindowDisplay>());

  station.SetTemp(60);
}
