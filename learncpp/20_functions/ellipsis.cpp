/*
 * LearnC++ 20.5 Ellipsis and why to avoid them
 *
 * copmile with make ellipsis
 *
 * */

#include <iostream>
#include <cstdarg>

/*
 * Ellipsis
 *
 * Allows variable number parameters to a function. It is specified with the ...
 * keyword. A function with an ellipsis must have at least one non ellipsis
 * argument and the ellipsis must come last. Think of it as an array of
 * parameters beyond argument list
 *
 * Use va_start to initialize the list. The second parameter is the first non
 * ellipsis parameter.
 *
 * Danger: Type checking is suspended with ellipsis.
 * */

using std::cout;

void func(int count, ...) {
    std::va_list list; // variable argument list
    va_start(list, count);
    for (int i {0}; i < count; ++i) {
        cout << va_arg(list, int) << ",";
    }
    cout << "\n";
    va_end(list);
}

int main() {
    func(5, 1, 2, 3, 4, 5);

    func(1, "hi"); // Interprets string as an int
}
