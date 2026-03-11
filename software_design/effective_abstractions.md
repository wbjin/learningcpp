# Effective Abstractions

## Dimensions of Abstractions
Abstractions can be qualified and assessed by
- Expressive power
- Complexity
- Obscurity
- Depth

### Expressive Power
The range of things you can do with an abstraction. 
Lower expressive power:
```c++
int sumVector(const std::vector<int>& nums) {
    int sum = 0;
    for (int n : nums) {
        sum += n;
    }
    return sum;
}
```
You can only do a sum of the vector.

Higher expressive power:
```c++
int sum = std::accumulate(nums.begin(), nums.end(), 0);
int product = std::accumulate(nums.begin(), nums.end(), 1,
    [](int a, int b) { return a * b; });
```
You can pass lambdas into the `std::accumulate` to express how you want to
accumulate elements of a vetor.

### Complexity
The difficulty of understanding and using the abstraction

Low complexity:
```c++
void printHello() {
    std::cout << "Hello\n";
}
```
The function doesn't have any arguments, side effects, or lifetime
implications.

High complexity:
```c++
void worker() {
    std::cout << "Running in thread\n";
}

int main() {
    std::thread t(worker);
    t.join();
}
```
The thread interface forces you to understand the thread lifecycle. It has side
effects such as starting a separate stream of execution. You also have to
understand how to chain abstractions together to use it correctly such as
calling `t.join()` to make sure the thread gets executed.

### Obscurity
The hidden knowledge or understanding you need to use an abstraction such as
understanding side effects or if there are unexpected ways to use an
abstraction. Ties in with complexity.

Low Obscurity:
```c++
int max(int a, int b) {
    return (a > b) ? a : b;
}
```
There aren't any hidden side effects.

High Obscurity
```c++
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```
If you were to use this macro with something like `MAX(x++, 5)`, `x++` would be
evaluated twice. There are unexpected side effects with the way macros work.

### Depth (Surface to Volume ratio)
The amount of complexity an abstraction hides.

Low depth:
```c++
int add(int a, int b) {
    return a + b;
}
```
There isn't much depth to this abstraction, it simply adds two numbers up.

High depth:
```c++
read(fd, buffer, sizeof(buffer));
```
The file I/O interface abstracts away many complexities such as which device
you are reading from (disk, NVMe, network card).

### Complexity and Expressive Power
Complexity and Expressive power in abstractions are often a tradeoff. You want
low complexity but high expressive power abstractions but that's often
challenging. Often times, high expressive power comes with high complexity.

### Obscurity and Opacity
Typically, we don't want obscurity with our abstractions. However, there's
often a tradeoff between complexity and obscurity where you can make thins
obscure which makes the abstraction less complex and easier to use.
Opacity is the knowledge you don't need to use an abstraction that's
appropriately hidden away. Opacity is generally desired and good abstractions
hide away details that you don't need.

### Depth
Abstractions should maximize the ratio between depth and surface area. The
depth is the complexity that an abstraction hides away while the surface area
is the cost of using that abstraction.

### Composition
Abstractions can be combined and used together in what we call composition.
Abstractions that are by themselves shallow can be chained together to create
more deep abstractions.
```c++
std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8};

auto pipeline =
    nums 
    | std::views::filter(is_even)                     // keep even numbers
    | std::views::transform([](int x) { return x * 3; }) // multiply by 3
    | std::views::filter(greater_than_ten)           // keep values > 10
    | std::views::transform(square)                  // square them
    | std::views::transform(label);                  // convert to strings
```

### Facade design pattern
Create a simple interface for a complex system. Instead of make a user interact
with many objects, a facade class wraps the complexity into a simpler
interface.
```c++
class CPU {
public:
    void start() { std::cout << "CPU starting\n"; }
};

class Memory {
public:
    void load() { std::cout << "Memory loading\n"; }
};

class Disk {
public:
    void read() { std::cout << "Disk reading\n"; }
};

class ComputerFacade {
    CPU cpu;
    Memory mem;
    Disk disk;

public:
    void startComputer() {
        cpu.start();
        mem.load();
        disk.read();
    }
};
```
Instead of needing to separately instantiate and call methods on `CPU`,
`Memory`, `Disk`, the `ComputerFacade` abstracts this all into a single object
and method.

## Coupling
Coupling is the cost of using an abstraction. The complexity that we reduce
with the abstraction should be less than the complexity of understanding the
abstraction.

Coupling is how strongly two "things" depend on each other. It's a measure of
how much one piece of code knows about or relies on another piece of code, ie
how much dependence there is between code (one way or two way). Generally, low
coupling is desired where you want to be able to change components
independently.

Coupling has many different types

### Mechanical
Dependence on the underlying environment. It is when software modules are
connected because of implementation mechanics rather than a meaningful
relationship. This can be things like dependence on specific data formats,
param ordering.

```c++
void log(int level, const std::string& message);
```
The caller must remember the correct ordering of parameters and can be misused
if not ordered correctly.
 
Mechanical coupling can also be the dependence on the runtime environment such
as TypeScript running on an interpreter and it depends on the correctness and
implementation of that interpreter.

### Structural
Dependence on availability of data and operations. It is when a module needs to
know the member variables or member functions of another module rather than
using a stable interface.

```c++
class User {
public:
    std::string name;
    int age;
};

void printUser(const User& user) {
    std::cout << user.name << " is " << user.age << " years old\n";
}
```
The `printUser` function is structurally coupled with the `User` class because
it needs to know its internal member variables. If a new member variable is
added to `User`, `printUser` would have to know that. Instead you can decouple
by having member functions that create the string representation and the caller
doesn't have to know the internal reprsentation of data.

### Behavioral
Dependence on the results or side effects of an operation. It is when module
depends on the correct usage pattern of another module.
```c++
class FileWriter {
public:
    void open() {
        std::cout << "File opened\n";
    }

    void write(const std::string& data) {
        std::cout << "Writing: " << data << "\n";
    }

    void close() {
        std::cout << "File closed\n";
    }
};

int main() {
    FileWriter writer;

    writer.open();
    writer.write("Hello");
    writer.close();
}
```
The user of `FileWriter` must call `open` before `write` and `close` before
terminating.

Behavioral coupling can be explicit like expecting the return value of a
function or implicit where there is a correct order of invoking methods of
another module.

### Semantic
Dependence on the meaning or intepretation of data or operations. It is when a
module depends on the meaning or assuptions made by another module rather than
just its interface.
```c++
void processOrder(int status) {
    if (status == 1) {
        std::cout << "Order approved\n";
    } else if (status == 2) {
        std::cout << "Order rejected\n";
    }
}
```
The caller of `processOrder` must under the semantics behind what `status`
means and what the valid values are.

### Impact of Coupling
The impact of coupling can be measured with the following factors
- Complexity: How complicated the dependence or shared knowledge is
- Distance: How far apart the coupled pieces live in terms of code and
developers
- Feedback: How reliable and quickly we can get feedback if there is a breaking
change
- Volatility: How likely it is to make a breaking change

These combine multiplicatively.

## Encapsulation Boundaries

Encapsulation is dividing parts of the code into units where within a unit,
parts directly interact with each other but from outside the units, the
individual parts operate as one through a well-defined interface.

The goal is to create "capsules" that encapsulate individual units that operate
together while keeping the abstraction and coupling manageable.

Encapsulation "units" can be different things like classes, structs, functions,
modules, or even entire services.

Encapsulated units often have to maintain an invariant for that unit to make
sense. Putting everything the invariant depends on into a single unit if often
the right choice as there is less distance between the coupled components.

```c++
class Stack {
private:
    std::vector<int> elements;
    int size = 0;   // invariant: size == elements.size()

public:
    void push(int value) {
        elements.push_back(value);
        size++;
    }

    int pop() {
        if (size == 0)
            throw std::runtime_error("Stack is empty");

        int value = elements.back();
        elements.pop_back();
        size--;
        return value;
    }

    int get_size() const {
        return size;
    }
};
```
It's easier to have `size` and `elements` in one unit rather than having it in
different units

Encapsulations can be too narrow or two wide. When it is too narrow, an
invarian may depend on something outside of the unit which means the user of
the abstraction has to maintain these different units together which ads
accidental complexity. If it is too wide, the encapsulation hides away too many
details and can burden the user with underestanding abstractions that aren't
necessary for what they are trying to accomplish.

```c++
class Account {
public:
    int balance = 0;
};

class WithdrawalLimiter {
public:
    int dailyLimit = 500;
};

Account account;
WithdrawalLimiter limiter;

account.balance -= 100;
limiter.dailyLimit -= 100;
```
Here, `Account` and `WithdrawalLimiter` must be maintained together because the
invariant spans across two encapsulation boundaries.

```c++
class FileManager {
public:
    void readFile(const std::string& path);
    void writeFile(const std::string& path, const std::string& data);
    void compressFile(const std::string& path);
    void encryptFile(const std::string& path);
};
```
This encapsulation includes too many different things that aren't necessarily
coupled with each other.
