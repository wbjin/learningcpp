cXX = g++
CXXFLAGS = -std=c++17 -Wall
EXT = .cpp
OUTEXT = .out

# Pattern rule: % matches the target name like `initialization`, `maybe_unused`
%: %$(EXT)
	$(CXX) $(CXXFLAGS) $< -o $@$(OUTEXT)

.PHONY: clean
clean:
	rm -f *$(OUTEXT)

