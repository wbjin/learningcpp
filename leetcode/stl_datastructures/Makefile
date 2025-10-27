CXX = g++
CXXFLAGS = -std=c++20 -Wall
EXT = .cpp
OUTEXT = .out


# Pattern rule: % matches the target name like `initialization`, `maybe_unused`
%: %$(EXT)
	$(CXX) $(CXXFLAGS) $< -o $@$(OUTEXT)

.PHONY: clean
clean:
	rm -f *$(OUTEXT)

