/*
 * LearnC++ 2.10 Introduction ot preprocessor
 *
 * copmile with make preprocessor
 *
 * */
#include <iostream>
using std::cout, std::endl;

/*
 * Preprocessor
 *
 * Preprocessor (usually part of the copmiler) goes through the source code and makes changes such as removing comments, adding new lines to end of each statement, and processing incldue directives
 *
 * Preprocessor outputs a translation unit. Translation unit is then copmiled into object files.
 *
 * The preprocessor looks for preprocessor directives (things that start with # and end with newline)
 *
 * When you do #include, it is replaced by the actual contents of the file by the preporcessor. This is then preprocessed as well for further nestesd #include
 * */

/*
 * Macro definitions (#define)
 *
 * Macros are a rule that defines how input text is converted into replacement output text. Macros can be object-like or function-like
 *
 * Function-like macros behave like normal functions
 *
 * Object-like macros repalce the identifier in the macro with the text you add
 *
 * Try to avoid macros for substitution text, write them in all uppercase if you do use them.
 *
 * #defines don't follow C++ scopes. A #define will be available anywhere in the file and any files that #include it
 *
 * #define is discarded once the preprocessor is finished. This means it is noy available in the compilation and linking step and are only available in the file you define it in and any files that incldue it
 * */

/*
 * Conditional compilation
 *
 * You can use macros to compile certain things if it is defined
 *
 * #ifdef checks whether the identifier has been previously defined with #define. If so the code in between the #ifdef and #endif is compiled
 * #ifndef is the opposite of #ifdef, if a macro is not defined, compile the code
 *
 * Anything between #if 0 is basically commented out and won't be compiled. Things between #if 1 will be compiled
 * */

#define MACRO "macro_text"
#define PRINT

int main() {
    cout << MACRO << endl;

#ifdef PRINT
    cout << "PRINT" << endl;
#endif

// This won't print because NOPRINT isn't defined
#ifdef NOPRINT
    cout << "NOPRINT" << endl;
#endif

#if 0
    cout << "WONT PRINT" << endl;
#endif
}
