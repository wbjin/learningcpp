# Priroity Queue `std::priority_queue`

Heap data structure in the C++ STL. It is by default a max heap. 

The template arguments of the constructor is the 
- T: Type of the data
- Container: The underlying container, default `std::vector<T>`
- Compartor: The comparator used to sort, default `std::less<typename Container::value_type>`

## Member objects
- Container (underlying container).
    - For vector, implementation defined but usually contains pointers to beginning of storage in heap, end of storage, end of actual data
- Comparison function object, comparison is done by calling `Compartor(T)`

## Initialization
- With range iterator
```
std::priority_queue<int> p(vec.begin(), vec.end());
```

## Methods
- Adding to priority queue:
```
void push(const T&)
void push(T&&) // rvalue (using std::move or something)
```
- Getting top of priority queue:
```
const& T top() const; // return reference
```
- Popping top value: `void pop()`

## Other stuff

You can define a custom comapartor,
```
struct Comparator {
    bool operator()(const Obj& x, const Obj& y) const {
         // < is max heap, > is min heap
        return x.first < y.first;
    }
};
...
std::priority_queue<Obj, std::vector<Obj>, Comparator>
```
or

you can define the < operator overload for an object
```
struct Event
{
    int priority{};
 
    friend bool operator<(Event const& lhs, Event const& rhs)
    {
        return lhs.priority < rhs.priority;
    }
};
...
std::priority_queue<Event> events;
``` 
