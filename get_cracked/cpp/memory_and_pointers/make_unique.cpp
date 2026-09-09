#include <memory>

namespace getcracked {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace getcracked

/*
 * typname... Args: This is a template parameter pack where Args reprsents zero or more types
 * For example, if you do make_unique<Widget>(42, 3.14, "hi"), T would Widget and Args would be
 * a pack of int, float, const char (&)[3]
 *
 * Args&&: This is a forwarding reference meaning it can accept both lvalues and rvalues
 * make_unique<Widget>(name) --> std::string&
 * make_unique<Widget>(std::string("hi")) --> std::string&&
 *
 * std::forward<Args>: Preserves the lvalue rvalue semantics. Inside the function, the rvalue
 * parameter becomes a named lvalue but we want to maintain the original value category
 * */
