#include <cstdio>

template <typename T, template <typename> class crtpType>
struct crtp {
	T &underlying() { return static_cast<T &>(*this); }
	const T &underlying() const { return static_cast<const T &>(*this); }

private:
	crtp() {}
	friend crtpType<T>;
};

template <typename T>
class Base : public crtp<T, Base> {
public:
	void interface() { this->underlying().interface_impl(); }

private:
  Base() = default;
  friend T;
};

class Derived1 : public Base<Derived1> {
public:
	Derived1() = default;

	void interface_impl() { std::printf("Derived1::interface_impl()\n"); }
};

class Derived2 : public Base<Derived2> {
public:
	Derived2() = default;

	void interface_impl() { std::printf("Derived2::interface_impl()\n"); }
};

template <typename T>
void example(Base<T> &b) {
	b.interface();
}

int main() {
	Derived1 d1 {};
	Derived2 d2 {};

	example(d1);
	example(d2);
}
