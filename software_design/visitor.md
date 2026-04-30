# Visitor Pattern

The visitor pattern allows you to add new operations to existing object
structures without modifying the object itself. Instead, you implemenet a
separate "visitor" that operates on the object. Instead of the object
containing a member function to do some operation, the object instead has a
method that "accepts" visitors. These visitors then implement an operation.

```C++
class Circle;
class Rectangle;

class Visitor {
public:
    // = 0 is a pure virtual function that must be implemented by the inheritor, ie there is no
    // default method
    // virtual keyword just says that this function can be overriden by the inheritor
    virtual void visit(Circle &c) = 0;
    virtual void visit(Rectangle &c) = 0;
};

class Shape {
public:
    virtual void accept(Visitor &v) = 0;
};

class Circle {
public:
    double radius = 5;

    void accept(Visitor &v) override {
        v.accept(*this);
    };
};

class Rectangle {
public:
    double width = 2;
    double height = 3;

    void accept(Visitor &v) override {
        v.accept(*this);
    };
};

class AreaVisitor {
public:
    void visit(Circle &c) {
        std::cout << 3.14 * c.radius * c.radius;
    }

    void visit(Rectangle &r) {
        std::cout << r.width * r.height;
    }
};

int main() {
    Circle c;
    Rectangle r;

    AreaVisitor v;

    c.accept(v);
    r.accept(v);
}
```

With the C++ implementation, there is a double dispatch. First, the `accept`
method dynamically dispatches based on the object type (`Circle` vs
`Rectangle`) to call the correct `accept` implementation of the object. Then,
`visit` dispatches statically and dynamically. The compiler picks `visit(Cirlce
&)` based on the type of `*this` but there is also dynamic dispatch based on
the runtime type of `Visitor &`.

The visitor pattern is useful when
- You have a stable object structure that can't or shouldn't change
- You need to add many different operations
- You want to avoid modifying existing classes

You should avoid the pattern when your object structure changes frequently (ie
the member variables) and you have to update each visitor every time that
happens.
