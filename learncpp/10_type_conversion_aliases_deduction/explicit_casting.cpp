/*
 * LearnC++ 10.6 Explicity type conversion and static_cast
 *
 * copmile with make explicity_casting
 *
 * */
#include <iostream>
#include <typeinfo>

using std::cout, std::endl;

/*
 * Ways to tell the compile how to do type conversion
 *
 * static_cast: Performs copmile-time type conversion. Safe
 * dynamic_cast: Performs runtime type conversion on pointers or references in an polymorphic hierarchiy. Safe
 * const_cast: Adds or removes const. Safe when only being used for adding const
 * reintepret_cast: Reinterpres the bit level representation of one type as if it was another type. Not safe
 * C-style casts. Performs combination of static_cast, const_cast, or reinterpret_cast. Not safe
 *
 * Try to avoid C-style casts and reintepret_cast as much as possible
 * */

int main() {
}
