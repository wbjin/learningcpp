# Facade Pattern

The facade pattern is used when you have some complex subsystem (could be
something that you don't manage) and you want to abstract away some of the
complexities of that subsystem to the rest of the codebase. Users can interact
with the facade's interface rather than the complicated, potentially variable
subsystem interface.

For example, consider a cloud provider SDK that has some controllers for
managing resources (`ComputeController`, `StorageController`,
`NetworkController`). This subsystem is hard to use and error prone. We want to
avoid the application having to redo the same complex chaining of APIs
everytime they want to create a node in the cloud provider.

The `CloudController` facade abstracts away the complexities through a unified
simpler interface in `CreateNode`. The rest of the application doesn't have to
depend on and understand the cloud SDK but only the simpler `CloudController`
interface.
