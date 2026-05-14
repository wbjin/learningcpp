# Flyweight Pattern

The flyweight pattern is used to save memory usage when allocating a large
number of objects. Objects have extrinsic state, state that is shared with
other objects of the same type, and intrinsic state, state that is unique to
the object. For example, for a `Particle` struct, the intrinsic state that is
unique to each object would be its position, veloctiy, mass. It's extrinsic
state that is likely shared among different instances of the object is
represented as `ParticleType` struct.

The flyweight pattern is usually used alongside the factory pattern. A
`ParticleTypeFactory` is responsible for allocating `ParticleType` instances
that have not been allocated yet. However, if there is already an allocation,
the existing object is used instead of creating another one. In order to create
a new `Particle` object, a `ParticleFactory` is also used as well. This
`ParticleFactory` abstracts away the fact that flyweight pattern is being used.

Note that `shared_ptr`s are being used to manage the allocations of
`ParticleType`. This means that once the number of references to a specific
`ParticleType` becomes 0, the deallocation happens automatically. Although,
with the existing code, that will never happen because the `types_` member
variable in `ParticleTypeFactory` will always maintain at least one reference.

The important thing is that even though we allocate many "Smoke" particles,
only one of `ParticleType` is allocated.
