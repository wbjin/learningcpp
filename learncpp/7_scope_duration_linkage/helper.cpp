/*
 * LearnC++ 7.7 External linkage ad variable forward declarations
 *
 * links with external_linkage
 *
 * */

#include <iostream>

using std::cout, std::endl;

// For external_linkage.cpp
int non_const {1}; // No need for extern for non const globals
extern constexpr int global {2};

void func() {
    cout << "helper::func\n";
}

// For inline.cpp
inline double pi() { return 3.14159; }
