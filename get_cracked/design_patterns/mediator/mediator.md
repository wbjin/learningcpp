# Mediator Pattern

The mediator pattern centralizes the communication between objects so that they
talk through an object instead of talking to each other directly. This
decouples the objects from one another making them more reusable. It is
essentially "glue code" wrapped in an object to manage dependencies exploding.

In this pattern, there is typically a base class mediator (`Mediator`) that
defines a communication interface and a concrete mediator derived class
(`LoginDialog`) that implements the glue code between various objects.

Components that you want to manage the communication between inherit from a
base class (`Component`) which implements a way to set the mediator. Concrete
classes such as `LoginButton`, `UsernameTextBox`, and `PasswordTextBox` call
the `Notify` interface of the meidator.

## C++ Notes
- You can actually do polymorphism through references and not just pointers. In
the `Notify` interface of the `Mediator` class, it takes in a `const
Component&` which is called from various concrete `Component` implementations
that pass in a `*this`. This causes an upcast and the component that called the
function will be viewed from a `Component` point of view instead of the
concrete implementation.
