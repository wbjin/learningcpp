### If statements
Assignment if statements will evaluate to true if the variable you are assigning to evaluates to true. For example, if x is 1 or not a nullptr.
```
if (x = func()) {
    // some code
}
```

constexpr if staements are evaluated at compile time. The following code is replaced with just the code in the true section
```
constexpr double g {9.8};
if constexpr (gravity == 9.8)
    cout << "Gravity is normal";
```

```
constexpr double g {9.8};
cout << "Gravity is normal";
```

### Switch statements
Switch statements are fast because usually they only allow integral values to switch on. Compilers usually implement switch statements with jump tables. Jump tables are kind o flike arrays where the integral value is used as the index to jump to the code instead of comparing with every value.

Use the `[[fall_through]]` attribute to tell the compiler to continue execution on following blocks
```
switch (2)
{
case 1:
    std::cout << 1 << '\n';
    break;
case 2:
    std::cout << 2 << '\n'; // Execution begins here
    [[fallthrough]]; // intentional fallthrough
case 3:
    std::cout << 3 << '\n'; // This is also executed
    break;
}
```