/*
 * LearnC++ 10.3 Numeric conversions
 *
 * copmile with make numeric_conversions
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * 5 basic types
 * Converting itnegral type to any other integral type (int to short, int to long, short to char)
 * Converting flaoting point type to any other floating poiny type (double to float, double to long double, etc...)
 * Converting floating piont to integral type (double to int, float to int, etc...)
 * Converting integral type to floating point type (int to float, int to double, etc..)
 * Converting integral type or floating point to bool (int to bool, float to bool)
 *
 * Numeric conversions can be unsafe meaning some values of the source may not be able to converted into an equal value in the destination type
 * */

/*
 * Narrowing conversions. Conversions where the destination type may not be able to hold all possible values of source type.
 *
 * Avoid narrowing conversions, use static_cast to explicitly change type if you have to
 *
 * Conversions with constexpr variables will check if the conversion retained the value of the data at compile time.
 * */
int main() {
    // Value preserving conversions. All possible source values can be represented by destination type.
    // You can always convert a value-preserving conversion into the original value and type
    int a {1};
    long b {a};

    short c {1};
    double d = c;

    // Reinrepretive conversions. Converte dvalue may be different than source value but no data is lost
    // Signed/unsigned conversions
    int e {-1};
    unsigned int f  = e;
    cout << f << endl; 
    int g = f; // Preserves data so you can cast back to original data
    cout << g << endl; 

    // Lossy conversions. Usafe numeric conversions where data may be lost during conversion
    // May be platform dependent. Example, int is typically 4 bytes and doubles are typically 8 bytes so it would be a safe conversion. However, if int and double are both 8 bytes, int to double would be lossy
    double h {1.2};
    int i = h; // data loss
    float j {1.2311131111111231}; // Precision lost because float can hold to this much precision
    
    // Narrowing conversion with constexpr
    constexpr int k {-5};
    // unsigned int l {k}; // Compile error
}
