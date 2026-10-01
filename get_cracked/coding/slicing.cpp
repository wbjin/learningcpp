#include <iostream>
#include <memory>

struct Account {
	int balance = 100;

	virtual int availableFunds() const { return balance; }

	virtual ~Account() = default;
};

struct OverdraftAccount : Account {
	int overdraftLimit = 500;

	int availableFunds() const override { return balance + overdraftLimit; }
};

// Passing by value slices the derived object.
void printByValue(Account account) { std::cout << account.availableFunds() << '\n'; }

// Passing by reference preserves the original object.
void printByReference(const Account& account) { std::cout << account.availableFunds() << '\n'; }

void printByValuePolymorphic(std::polymorphic<Account> account) {
  std::cout << account->availableFunds() << "\n";
  account->balance = 0;
  std::cout << account->availableFunds() << "\n";
}

int main() {
	OverdraftAccount account;

	// copies into the function, the overDraftLimit part of the derived class is not copied
	printByValue(account);
	// no copying, a reference to the underlying object
	printByReference(account);
  std::cout << account.overdraftLimit << '\n';  // Still 500

	std::polymorphic<Account> account2(std::in_place_type<OverdraftAccount>);
	printByValuePolymorphic(account2);
  std::cout << account2->availableFunds() << "\n";
}
