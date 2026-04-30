# Error Handling

## Exception
Change the control path of the program from the regular return to outside of
the functions until a `catch`. 

Exceptions can be typed where you can throw a specific exception type and catch
specific exception types. Some language compilers checks if a typed exception
that is thrown is caught in a `catch` statement when the function is called.

## Undefined behavior
Not the problem of the callee if a precondition is violated (ie a require
clause)

## Errors as Values
No separate control flow for an error. Function returns something that
indicates an error.

## Look Before You Leap
Assume that operations can fail and know the preconditions before using an
abstraction.

## Easier to Ask for Forgiveness than Permission
If an abstraction has an error, handle it instead of having to check for
preconditions.

## Taxanomy of Exceptionality
- Fatal: An exception is thrown because of a fatal system error (eg out of
memory)
- Preventable: An exception is thrown because of a programming error that could
be fixed
- Exogenous: An exception is thrown because something out of our control fails
- Vexing: An exception is thrown because of poor design or wrong encapsulations

## Exception safety
The state that the program is left in when there is an execption
- No Fail guarantee: The function always completes normally
- Strong guarantee: The function can't complete normally and no state is
modified
- Basic guarantee: If function can't complete normally, no invariants are
broken
- No guarantee
