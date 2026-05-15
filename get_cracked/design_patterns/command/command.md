# Command Pattern

The command design pattern allows you to encapsulate operations as a standalone
object that can be passed around, stored, queued, logged, undone, etc. For
example, the `Account` class provides an API for a bank account with a way to
credit or debit money. Instead of doing `account.Debit` or `account.Credit`, we
can wrap these actions in `DebitCommand` and `CreditCommand`. 

In this example, the `Account` class is referred to as the "receiver". This is
the class that has the underlying implementation that the command relies on.
`DebitCommand` and `CreditCommand` are concrete command objects that implement
the `Execute` and `Undo` interfaces.

`AccountManager` is what we call the "invoker". It invokes the commands that
are passed into `Execute` (different from the execute in the commands). It
maintains a history of commands that are applied to a specific account.
(Perhaps a better design is for the account manager to create and invoke the
commands instead of having them passed in). Note that in `main`, the client
creates concrete `Command` objects and passes them in as actions instead of
calling a function like `Debit`. `AccountManager` also exposes a rollback that
invokes the `Undo` method on the most recent command and pops it from the
history.
