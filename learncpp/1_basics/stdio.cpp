/*
 * LearnC++ 1.5 Introduction to iostream: cout, cin, and endl
 *
 * copmile with make stdio
 *
 * */
#include <iostream>

using std::cout, std::endl, std::cin;

int main() {
    // std::cout is buffered. This doesn't immediately print to stdandard out. Instead, it is added to a buffer and the buffer is flusehd to standard out periodically
    cout << "Hi\n";

    // std::endl outputs a newline and it flushes the buffer.
    cout << endl;

    int x{};
    // std::cin is also buffered. Each individual character is added to the input buffer with the enter being storead as a \n character. The >> operator removes the characters from the input buffer
    // std::cin doesn't add whitespace (space, tabs, newlines) to the buffer
    // >> blocks while there is nothing in the input buffer
    // >> copy assigns the value into the variable
    // if >> fails, the variable is assigned the value 0 and any future extraction fails until std::cin is cleared
    cin >> x;
}
