# Polymorphism

## Kinds of Polymorphism

### Subtype Polymorphism
Objects of different subclasses can be treated as ojects of a common base
class. The correct method to call is chosen at runtime. This is the classic use
of polymorphism with inheritance and virtual functions.
```c++
class Shape {
public:
    virtual void draw() {
        std::cout << "Drawing a shape\n";
    }
};

class Circle : public Shape {
public:
    void draw() override {
        std::cout << "Drawing a circle\n";
    }
};

class Square : public Shape {
public:
    void draw() override {
        std::cout << "Drawing a square\n";
    }
};

int main() {
    Shape* s1 = new Circle();
    Shape* s2 = new Square();

    s1->draw();
    s2->draw();
}
```
Both `Circle` and `Square` are treated as type `Shape*`. The correct method
(`Cricle::draw` or `Square::draw`) is selected at runtime in C++ with virtual
tables and virtual table pointers. For each class with a `virtual` keyword or a
method implementing a `virtual` function, the compiler creates a virtual table
which is an array of function pointers to the method to invoke. This virtual
table is pointed to by the virtual table pointer in each class. When you call
`s->draw()` on a virtual class like `Circle`, the compiler looks at `Circle`'s'
`vptr`, follows it to the table, and selects the correct function to invoke.

With subtype polymorphism, you don't need to know the specific subtype when
using them. However, you do need to know it when creating them.

### Adhoc Polymorphism
Instead of a base interface, instead there is a union of subtypes as well as a
union of the data encapsulation that represents the subtype. It is when there
are multiple behaviors for the same conceptual operation depending on the input
type. The programmer also controls how to dispatch to the "correct" operation.
```c++
struct EmailData {
    std::string recipient;
    std::string subject;
    std::string message;
};

struct SMSData {
    std::string phoneNumber;
    std::string message;
};

using NotificationVariant = std::variant<EmailData, SMSData>;

void send(const NotificationVariant& notification) {
    if (std::holds_alternative<EmailData>(notification)) {
        const auto& email = std::get<EmailData>(notification);
        std::cout << "Sending email to " << email.recipient << "\n";
    }
    else if (std::holds_alternative<SMSData>(notification)) {
        const auto& sms = std::get<SMSData>(notification);
        std::cout << "Sending SMS to " << sms.phoneNumber << "\n";
    }
}
```

### Parametric Polymorphism
Parametric polymorphism is when code is written generically for all types so it
can without changing the implementation. In C++, this is often done with
templates
```c++
template <typename T>
T add(T a, T b) {
    return a + b;
}

int main() {
    add(2, 3);
    add(2.5, 3.5);
}
```

## Composition vs Inheritance
Composition combines objects together to build some behavior. Polymorphism uses
a common interface to allow different behaviors. In composition, a class may
contain another class that performs some subset of work. This is a "has-a"
relationship.

A common way composition is used is with the strategy pattern.
```c++
class PaymentStrategy {
public:
    virtual void pay(int amount) = 0;
    virtual ~PaymentStrategy() = default;
};

class CreditCardPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        std::cout << "Paid $" << amount << " with credit card\n";
    }
};

class PayPalPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        std::cout << "Paid $" << amount << " with PayPal\n";
    }
};

class Checkout {
private:
    PaymentStrategy* strategy;

public:
    Checkout(PaymentStrategy* strategy) : strategy(strategy) {}

    void processPayment(int amount) {
        strategy->pay(amount);
    }
};

int main() {
    CreditCardPayment card;
    PayPalPayment paypal;

    Checkout checkout1(&card);
    checkout1.processPayment(50);

    Checkout checkout2(&paypal);
    checkout2.processPayment(30);
}
```
The `Checkout` class takes in the `PayPalPayment` or the `CreditCardPayment`
strategy. This injects the dependency where instead of the `Checkout` class
creating payment strategies, the user can inject specific strategies. By
injecting interfaces, we can invert dependencies. Instead of `Checkout` being
dependent on a single payment strategy, instead it is now dependent on an
interface `PaymentStrategy`. Different types such as `PayPalPayment` are now
dependent on this.

## Extensibility

### Liskov Substitution Principle
Put simply, any proparety provable about a basetype should hold for the
subtype. This means things like the method signatures should be the same in the
subtype where the return type and parameter types are a subset of the basetypes
return and parameter types and its methods shouldn't change any postconditions,
preconditions, and invariants. Essentially, if you were to substitute a subtype
for a basetype, the way you use it should not change.

For method signatures that are implemented or overriden by a subtype
- The return type should be covariant which means the return type should be the
same or a subtype of the base method.
- The parameter type should be the same or a supertype of the base method
- Exceptions should also be coveriant.

For behavior
- The postcondition should not be weakened, the subtype should not weaken
promises made by the base method.
- The precondition should not be strengthened, the subtype should not assume
more than the base method.
- Does not weaken invariants
- Maintain the history rule

### Open Closed Principle
Software should be open for extension but closed for modification. Essentially,
adding features is okay, avoid modifying features or abstractions requiring
changes to other parts of the system.
