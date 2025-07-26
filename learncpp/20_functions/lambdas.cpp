/*
 * LearnC++ 20.6 Introduction to lambdas
 *
 * copmile with make lambdas
 *
 * */

#include <numeric>
#include <iostream>
#include <vector>

using std::cout;

/*
 * Lambdas
 *
 * Lambdas are anonymous functions that allows us to define functions inside
 * another function. Lambdas don't have an identifier by default.
 *
 * The syntax is
 * [capture clause] (parameters) -> return type {
 * }
 *
 * The return type can often be ommited because it wil be infered as auto.
 *
 * You can store a lambda in a named variable
 *
 * Lambdas don't actually have a type that we can explicitly use. The compiler
 * generates a unique type just for that lambda. Lambda's also aren't actually
 * functinos. They are functors, objects that contain an overloaded operator()
 * that allows it to be called like a function.
 *
 * A lambda with an empty capture clause can be used like any other function
 * pointer. This means we can pass it into anywhere that has a std::function or
 * auto.
 *
 * Generic lambdas are lambda functions with one or more auto parameters. This
 * allows it ot be used with a variety of types. The compiler infers the
 * parameter types that needed for the calls. auto in the context of lambdas are
 * esentially template parameters.
 * */

/*
 * Lambda captures
 *
 * The capture clause in a lambda function is used to indirectly give a lambda
 * access to variables available in the surrounding scope. You can list the
 * entities that you want it to hae access to.
 *
 * The variable is cloned into the inside the lambda.
 *
 * The caputres aren't actually variables but functors. When compiler sees a
 * lambda, it creates a custom object with each captured variable becoming a
 * data member of that object. When the lambda definition is encountered at
 * runtime, the members of the lambda are initialized at that poin.
 *
 * Caputres are by default const. Need to sue the mutable keyword. Capture
 * values are persisted across multiple calls to the lambda. You can also
 * caputre by reference with the standard & keyword in front of the capture
 * variable. You no longer a mutable variable to modify that capture.
 *
 * Default caputures caputres all variables that are mentioned in the lambda. It
 * is specified with the = keyword in the capture clause. You can make it a
 * refernece with &.
 *
 * You can also define a variable in the capture so that it is only accessible
 * by the lambda. You can do so by declaring a variable in the capture with the
 * list initialiation {} syntax.
 * */

int main() {
    std::vector<int> a {1, 2, 3, 4, 5};
    int b {std::accumulate(a.begin(), a.end(), int {}, [](int accum, int n) {
        accum += n;
        return accum;
    })};
    cout << b << "\n";

    auto add {
        [](int accum, int n) {
            return accum + n;
        }
    };
    int c {std::accumulate(a.begin(), a.end(), int {}, add)};
    cout << c << "\n";

    int exclude {5};
    // lambda has access to the exclude variable outside of its scope
    int d {std::accumulate(a.begin(), a.end(), int {}, [exclude](int accum, int n) {
        // exclude = 1; // const
        if (n != exclude)
            accum += n;
        return accum;
    })};
    cout << d << "\n";

    auto mutable_add {
        [exclude](int accum, int n) mutable {
            exclude = n;
            if (n != exclude)
                accum += n;
            return accum;
        }
    };
    int e {std::accumulate(a.begin(), a.end(), int {}, mutable_add)};
    cout << e << "\n";

    auto add2 {
        [=](int accum, int n) mutable {
            exclude = n;
            if (n != exclude)
                accum += n;
            return accum;
        }
    };
}
