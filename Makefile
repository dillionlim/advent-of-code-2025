# Usage:
#   make           # Compiles and runs the solution
#   make compile   # Just compiles (formerly 'make build')
#   make run       # Compiles and runs (same as default)
#   make new day=5 # Creates Day 05 files
#   make clean     # Removes the build directory

BUILD_DIR := build
EXEC := $(BUILD_DIR)/aoc
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Release

.PHONY: all run compile new clean

all: run

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Makefile: | $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. $(CMAKE_FLAGS)

compile: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR)

run: compile
	@echo "\n>>> Running AoC 2025 <<<"
	@./$(EXEC)

clean:
	rm -rf $(BUILD_DIR)

new:
ifndef day
	$(error Usage: make new day=<number>)
endif
	@day_int=$$(echo $(day) | sed 's/^0*//'); \
	if [ -z "$$day_int" ]; then day_int=0; fi; \
	day_pad=$$(printf "%02d" $$day_int); \
	target_dir="src/days/day$$day_pad"; \
	if [ -d "$$target_dir" ]; then \
		echo "Error: $$target_dir already exists"; \
		exit 1; \
	fi; \
	mkdir -p "$$target_dir"; \
	touch "$$target_dir/a.in"; \
	touch "$$target_dir/b.in"; \
	printf '#include "aoc.hpp"\n\nstruct Day%s : public Solution {\n    std::string part1(const std::string& input) override {\n        return "todo";\n    }\n\n    std::string part2(const std::string& input) override {\n        return "todo";\n    }\n};\n\nAOC_REGISTER(%s, Day%s)\n' $$day_pad $$day_int $$day_pad > "$$target_dir/solution.cpp"; \
	printf '[[test_cases]]\npart = 1\nexpected = "123"\ninput = """\nexample\n"""\n' > "$$target_dir/test.toml"; \
	echo "Created $$target_dir"; 
