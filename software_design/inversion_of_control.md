# Inversion of Control

Inversion of control is when the control flow is established in the opposite
order of dependency, ie "Don't call us, we'll call you".

```c++
#include <iostream>

class Logger {
public:
    virtual void log(const std::string& msg) = 0;
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& msg) override {
        std::cout << msg << std::endl;
    }
};

class Application {
private:
    Logger& logger;

public:
    Application(Logger& logger) : logger(logger) {}

    void run() {
        logger.log("Application running");
    }
};

int main() {
    ConsoleLogger logger;
    Application app(logger);

    app.run();
}
```
This is a classic pattern where `main` depends on `Application`. However, it is
the `Application` that determines the control flow of when `ConsoleLogger` is
invoked.

## Observer Pattern
This is one common way to implement inversino of control. An observed entity
maintains a list of observers that it invokes when some change in state
happens. The observers implement a well known interface and the observed entity
invokes callbacks to let the observers know about some change.

```c++
class Observer {
public:
    virtual void update(int temperature) = 0;
};

class PhoneDisplay : public Observer {
public:
    void update(int temperature) override {
        std::cout << "Phone display updated: " << temperature << "°C\n";
    }
};

class Logger : public Observer {
public:
    void update(int temperature) override {
        std::cout << "Logging temperature: " << temperature << "°C\n";
    }
};

class TemperatureSensor {
private:
    std::vector<Observer*> observers;
    int temperature;

public:
    void addObserver(Observer* obs) {
        observers.push_back(obs);
    }

    void setTemperature(int temp) {
        temperature = temp;

        for (auto obs : observers) {
            obs->update(temperature);
        }
    }
};

int main() {
    TemperatureSensor sensor;

    PhoneDisplay phone;
    Logger logger;

    sensor.addObserver(&phone);
    sensor.addObserver(&logger);

    sensor.setTemperature(25);
}
```
`PhoneDisplay` and `Logger` implement a well known interface inheriting off of
`Observer`. `TemperatureSensor` knows about this well known interface and
whenver there is an event internally such as `setTemperature`, it invokes the
callbacks implemented with the `update` method.

## Pub/Sub pattern
This is another way to implement inversion of control where instead of the
observed entity managing the observers, a separate "broker" manages the
observers and the observed entity simply has to let the broker know about
events. The broker organizes events by topics and observers/subjects can
publish to specific topics via the broker. The broker will broadcast events to
subscribers registered to that topic. This is effective when you don't want
filtering or broadcasting logic be a dependency on your observed entity.

```c++
class Subscriber {
public:
    virtual void onEvent(int temperature) = 0;
};

class PhoneDisplay : public Subscriber {
public:
    void onEvent(int temperature) override {
        std::cout << "Phone display: " << temperature << "°C\n";
    }
};

class Logger : public Subscriber {
public:
    void onEvent(int temperature) override {
        std::cout << "Logging temperature: " << temperature << "°C\n";
    }
};

class Broker {
private:
    std::unordered_map<std::string, std::vector<Subscriber*>> topics;

public:
    void subscribe(const std::string& topic, Subscriber* sub) {
        topics[topic].push_back(sub);
    }

    void publish(const std::string& topic, int data) {
        for (auto sub : topics[topic]) {
            sub->onEvent(data);
        }
    }
};

class TemperatureSensor {
private:
    Broker& broker;

public:
    TemperatureSensor(Broker& broker) : broker(broker) {}

    void setTemperature(int temp) {
        broker.publish("temperature", temp);
    }
};

int main() {
    Broker broker;

    PhoneDisplay phone;
    Logger logger;

    broker.subscribe("temperature", &phone);
    broker.subscribe("temperature", &logger);

    TemperatureSensor sensor(broker);

    sensor.setTemperature(25);
}
```
Instead of `TemperatureSensor` managing the different subscribers, the
`TemperatureSensor` only depends on the `Broker`. The `Broker` abstracts away
the filtering logic such as topics and the mechanism of broadcasting.
