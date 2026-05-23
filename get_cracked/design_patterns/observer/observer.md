# Observer Pattern

The observer pattern is used to enable objects to react to a change in another
object without tightly coupling the two. It is sometimes known as the publisher
subscriber (PubSub) pattern as well though this may vary depending on where you
look at. The Observer/PubSub patterns usually has subjects/topics that
subscribers can selectively choose to react to.

`Observer` defines a base interface for all observers/subscribers to inherit
from. This interface usuallly has an `Update` method that the publisher invokes
whenever there is a change in internal state. `WeatherStation` is the publisher
in this example. Whenever, `SetTemp` is invoked, it invokes the `Update` method
on the subscribers.

In the PubSub pattern, there is a separate object called the Publisher. The
observed object (eg `WeatherStation`) calls a method on the publisher such as
`Publish`. The Publisher is then responsible for calling the correct
subscribers. In the PubSub pattern, subscribers are able to subscribe to
specific subjects/topics such as "when `SetTemp` is called", or when another
type of event happens so that the subscrber is only invoked when it wants to be
invoked.
