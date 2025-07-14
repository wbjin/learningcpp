/*
 * LearnC++ 14.16 Converting constructors and explicit keyword
 *
 * copmile with make converting_ctor
 *
 * */

#include <iostream>

using std::cout, std::endl;

/*
 * User defined conversions
 *
 * User defined objects don't have the built in conversion rules that
 * fundamental types have. For example, you can't implicitly convert an int to a
 * struct without explicitly defining the conversion. However, it's not good
 * practice to have user defined implicit conversions because it's hard to
 * understand in a big codebase.
 * */

/*
 * explicit keyword
 *
 * Tells the copmiler to not use this constructor for a converting constructor.
 * A constructor marked as explicit can't be used for copy initialization or
 * copy list initializaiton, cannot be used to do implicit conversions. This
 * applies to implicit conversions into functions and return values.
 *
 * Best practice for using explicit
 * - Make constructor with single non default argument explicit
 *
 * Don't make these explicit
 * - Copy and move constructors
 * - Default constructors with no parameters
 * - Constructors that only accept multiple arguments
 * */

class Name {
public:
    Name(const std::string_view name) : 
        _name(name) {}

    explicit Name (const char* name) : 
        _name(name) {}

    const std::string& get_name() const { return _name; }

  private:
    std::string _name{};
};

void print(const Name& name) {
    cout << name.get_name() << "\n";
}

int main() {
    //print("Joe"); // Causes a compiler error because this is a C string literal,
                // the concverting constructor takes in a string view. There
                // needs to be a conversion from string literal to Name or we
                // can explicilt pass in a string view.
    using namespace std::literals;
    print("Joe"sv);

    // print("Joe"); // compiler error if explicit in front of Name c string constructor
}
