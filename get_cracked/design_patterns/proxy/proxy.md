# Proxy Pattern

The proxy pattern is used to add additional control over an interface. Instead
of exposing an abstraction directly to the user, the proxy wraps around an
abstraction and provides the same interface but with code that wraps around it.
It provides additional control over the access of an object so that you can do
things like lazy loading, additional logging, etc.

`DirectXRenderEngine` is a third party library that exposes a `Render`
interface. This object is powerful but takes a long time to initialize and is a
memory hog so we want the application to avoid allocating it until it
absolutely has to. `DirectXRenderEngineProxy` allows this by wrapping around
`DirectXRenderEngine` and providing the same `Render` interface but with lazy
loading.
