/*
 * LearnC++ 10.2 Floating point and integral promotion
 *
 * copmile with make numeric_proomotion
 *
 * */
#include <iostream>

using std::cout, std::endl;

/*
 * Numeric promotion
 * Conversion of narrower numeric types like char to wider numeric types like int or double.
 * Why? Some architectures perform better on certain data widths (like 32 bit or 64 bit systems). For protability and for good performance, promotion might be needed
 *
 * Promotions are value-preserving (safe-conversion). Every possible source value will keep its original value in the destination type.
 *
 * Numeric promotions fall in two types, floating point promotion and integral promotions
 *
 * For floating point promotion, there is only float to double
 *
 * For integral promotion, there are
 * signed char or signed short to int
 * unsigned char, char8_t, and unsigned short converted to int if int wide enough or unsigned int otherwise
 * bool converted to int with false being 0 and true being 1
 * */

void floating_point_promotion(double x) {
    cout << x << "\n";
}

void int_promotion(int x) {
    cout << x << "\n";
}

int main() {
    floating_point_promotion(2.1f); // Float promoted to double
    floating_point_promotion(2.1); // No conversion
    
    // bool, char, signed/unsigned char, signed/unsigned short promoted to int
    int_promotion(3); // No conversion
    int_promotion('a');
    int_promotion(true);
    short s{2};
    int_promotion(s);
}
