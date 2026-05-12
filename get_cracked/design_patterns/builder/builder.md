# Builder Pattern

Compile with 
```bash
g++ main.cpp -std=c++20-o main
```

The builder pattern is used when you have a complex object. Often times, these
comple objects have constructors with dozens of parameters, many of which are
usually set to the default value. Instead of building the object in one step in
the constructor, the builder pattern allows the user to build and object step
by step with method calls dedicated for initialization. 

`HTTPRequest` is a class that represents an HTTP request. Suppose it contains
many internal metadata that is public and can be set from outside of the class
(or it declares the builder as a friend class that can access its private
members). Instead of building `HTTPRequest` directly, users rely on
`HTTPRequestBuilder` instead to handle the complex initialization logic.
`HTTPRequestBuilder` uses public setter methods that allows the user to
incrementally build a request. Note that these methods return
`HTTPRequestBuilder&` which allows the chaining of operations like
`SetMethod().SetUrl()`. The builder also exposes a `Build` method. This is
responsible for validating that the object that is being built is valid and
actually returning the object to the user.

Note that with the builder pattern, there is an idiom where we have a
"director". The director is responsible for chaining together the method calls
to the builder to create commonly used objects, ie it contains the recipe of
how to create an object within its class implementation. This also means that
the builder needs a base class interface that concrete builders derive from. I
chose to omit this because it seems like overkill.
