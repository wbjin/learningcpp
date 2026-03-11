# Programming Paradigms

## Imperative vs Declarative
Imperative: Specify what you want to do, how you want to do it. Can think of it
as controlling the ASM instructions that are ultimately executed by controlling
operations, control flow changes, data loads, etc.
```c++
std::vector<int> nums = {1, 2, 3, 4, 5};

int sum = 0;
for (int i = 0; i < nums.size(); i++) {
    sum += nums[i];
}
```

Declarative: Specifies what the result should be without controlling how it is
done. You describe a desired outcome but how you get that outcome is not up to
your control.
```c++
std::vector<int> nums = {1, 2, 3, 4, 5};
int sum = std::accumulate(nums.begin(), nums.end(), 0);
```

Structured imperative: Impose restrictions on imperative programs to make
programming easier. An example is using `goto` instead of `for` loops. Modern
languages have language features that allow for more structure.

Procedural imperative: Introduce subprocedures (functions) to decompose the
problem and make each component more manageable.

## Object Oriented
Builds on top of procedural, structured, imperative programming. The key ideas
are
- Encapsulation of data and methods
- Interact with objects via message passing, essentially calling methods on an
object to manipulate it instead of changing the internal data directly
