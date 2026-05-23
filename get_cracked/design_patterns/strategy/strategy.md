# Strategy Pattern

The strategy pattern allows you to have various differnet implementations of a
behavior with each implementation encapsulated in a class. It is useful when
you want to add a different implementation of an exisitng behavior.

A strategy usuaully has a base interface such as `RouteStrategy`. The different
implementations of the strategy (`FastestRoute`, `ShortestRoute`) inherit from
the base class.

The user of the strategy, `Navigator` does not care about the different
implementations of the strategy. It only cares about the interface and the
output.
