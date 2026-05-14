# Decorator Pattern

The decorator pattern allows you to add behavior to objects at runtime. You can
think of it as a matryoshka doll where there are many layers of objects, each
with the same interface. These layers can be added at runtime using
composition.

`Exporter` base class defines the interface for an exporter that does something
with some payload. `NullExporter` is an exporter that doesn't do anything with
this payload. This can be considered the "inner" most layer of the matryoshka
doll.

Various decorators inherit from the `Exporter` base class to implement the same
`Export` API. However, each of these decorators have a `exporter_` private
member that points to the next exporter or the "layer" that is to be called.
Note that another way to have done this was to have something like a
`Decorator` class that inherits from the `Exporter` class that defines the
interface for future decorators. This can move the redundant `exporter_`
declarations into one base class.

Various decorators are implemented (`DiskExporterDecorator`,
`NetworkExporterDecorator`). These decorators inherit the same `Export`
interface but internally also call the next layer.

The client first initializes a `NullExporter` instance. In order to add more
layers, this is passed into the constructor of future decorators. Note that you
can create any layering at runtime. Decorators are also a really good way to
implement pipelines.
