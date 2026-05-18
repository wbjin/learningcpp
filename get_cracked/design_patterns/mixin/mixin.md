# Mixin Pattern

Mixin is a pattern that allows you to create reusable behaviro by extending the
interface of an existing base class. Mixin and CRTP achieve the same goal but
through different methods. The biggest difference is the relationship between
the extension and the base class.

In CRTP, your base type opts into the extension (by having to include a type
parameter of the extension in `RepeatPrint`). Polymorphism is done through
compile time downcasting where the extension is built into the class that
inherits from the base class (`Name`).
```C++
template<typename Printable>
struct RepeatPrint {
    void repeat(unsigned n) {
        while(n--) {
            static_cast<Printable const&>(*this).print();
        }
    }
};

class Name : public RepeatPrint<Name> {
public:
    void print() const {...}
};
```

In mixin, the extended functionality wraps around your base class so the
existing class remains unchanged. For example, `User` and `Order` remain
unchanged when new behavior is added with mixins (`Timestampped`). The
`Timestampped` mixin extends the type `T` class by adding a
`printWithTimestamp` method. `Timestampped` has access to the base class
methods and the object of type `Timestampped` also has access to the base class
methods (you can call `print`). Note that `Timestampped` takes in the type `T`
as a constructor so you have to first create the `User` or `Order` objects
first to use `Timestampped`.

Another option to the `Timestampped` constructor is the `Serializable`
constructor. `Serializable` uses a forwarding constructor. A forwarding
constructor accepts arbitrary constructor arguments and forwards it to the base
class `T`.
- `template<typename... Args>`: This is a template parameter pack where
`Args...` means zero or more types. If the constructor is called with
`Serializable<User>("Bob")`, `Args = {std::string}`. If it is
`Serializable<User>("Bob", 25, true)`, `Args = {std::string, int, bool}`.
- `Args&& args`: This is a function parameter pack that captures all of the
arguments into one token `args`. This preserves lvalue and rvalues
- `std::forward<Args>(args)`: Maintains lvalue and rvalues because with
variables (`args`) always become lvalues
- `...`: This after the `std::forward` expands the parameter pack

You can also stack mixins as shown with `Serializable<Timestampped<User>>`.

CRTP
- Impacts definition of the existing class
- User code uses the base class instead of the derived classes

Mixin:
- Base class untouched
- User code uses the derived class directly with access to base class methods
