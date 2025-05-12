cXX = g++
CXXFLAGS = -std=c++17 -Wall
EXT = .cpp
OUTEXT = .out

.PHONY: external_linkage
external_linkage:
	$(CXX) $(CXXFLAGS) external_linkage.cpp helper.cpp -o external_linkage.out

.PHONY: inline
inline:
	$(CXX) $(CXXFLAGS) inline.cpp helper.cpp -o inline.out

# Pattern rule: % matches the target name like `initialization`, `maybe_unused`
%: %$(EXT)
	$(CXX) $(CXXFLAGS) $< -o $@$(OUTEXT)

.PHONY: clean
clean:
	rm -f *$(OUTEXT)

