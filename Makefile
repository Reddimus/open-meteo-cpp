# Open-Meteo C++ SDK - Root Makefile
# Wraps CMake for convenient day-to-day workflow

BUILD_DIR := build
CPP_AUTO_AUDIT := python3 tools/cpp_auto_audit.py
CMAKE := cmake
NPROC := $(shell nproc 2>/dev/null || echo 4)

.PHONY: all build debug test lint clean configure help format coverage \
	run-forecast run-historical run-geocoding run-air-quality

# Default target
all: build

# Configure CMake (if needed)
configure:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Configure debug build
configure-debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build all targets
build: configure
	@$(CMAKE) --build $(BUILD_DIR) -j$(NPROC)

# Debug build
debug: configure-debug
	@$(CMAKE) --build $(BUILD_DIR) -j$(NPROC)

# Run tests
test: build
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Run linting (clang-format check)
lint:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "Checking code formatting..."; \
		find src include tests examples \( -name '*.cpp' -o -name '*.hpp' \) -print0 | \
			xargs -0 clang-format --dry-run --Werror && \
		echo "Format check passed."; \
	else \
		echo "clang-format not found. Install clang-format to run lint."; \
		exit 1; \
	fi
	$(CPP_AUTO_AUDIT)


# Format code in place
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "Formatting code..."; \
		find src include tests examples \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 clang-format -i; \
		echo "Done"; \
	else \
		echo "clang-format not found. Install clang-format to format code."; \
		exit 1; \
	fi

# Code coverage (requires lcov)
coverage:
	@mkdir -p build-coverage
	@cd build-coverage && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug -DOPEN_METEO_ENABLE_COVERAGE=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@$(CMAKE) --build build-coverage -j$(NPROC)
	@cd build-coverage && ctest --output-on-failure
	@lcov --capture --directory build-coverage --output-file build-coverage/coverage.info --ignore-errors mismatch
	@lcov --remove build-coverage/coverage.info '/usr/*' '*/build-coverage/_deps/*' --output-file build-coverage/coverage_filtered.info --ignore-errors unused
	@genhtml build-coverage/coverage_filtered.info --output-directory build-coverage/coverage-report
	@echo "Coverage report: build-coverage/coverage-report/index.html"

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR) build-coverage
	@echo "Cleaned build directory"

# Run examples
run-forecast: build
	@./$(BUILD_DIR)/examples/example_forecast

run-historical: build
	@./$(BUILD_DIR)/examples/example_historical

run-geocoding: build
	@./$(BUILD_DIR)/examples/example_geocoding

run-air-quality: build
	@./$(BUILD_DIR)/examples/example_air_quality

# Help
help:
	@echo "Open-Meteo C++ SDK Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make build           - Configure and build the SDK (Release)"
	@echo "  make debug           - Configure and build the SDK (Debug)"
	@echo "  make test            - Run tests"
	@echo "  make lint            - Check code formatting"
	@echo "  make format          - Format code in place"
	@echo "  make coverage        - Generate code coverage report (requires lcov)"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make help            - Show this help"
	@echo ""
	@echo "Examples:"
	@echo "  make run-forecast    - Run forecast example"
	@echo "  make run-historical  - Run historical weather example"
	@echo "  make run-geocoding   - Run geocoding example"
	@echo "  make run-air-quality - Run air quality example"

# Markdown linting
lint-md:
	npx markdownlint-cli2 "**/*.md"

format-md:
	npx markdownlint-cli2 --fix "**/*.md"
