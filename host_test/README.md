# FloatSwitch Host Tests

This directory contains host-based tests for the `FloatSwitch` component. Host testing allows you to run unit tests on your development machine (Linux) instead of the target microcontroller, enabling faster development cycles and the use of the Google Test (GTest) framework.

## Test Scope

The tests cover individual components (Unit Tests) and logic verification. We aim for high coverage across all critical float switch logic, including:

- Electrical signal translation (LOW/HIGH active levels)
- Debouncing logic (majority-vote algorithm)
- Power management and wake-up conditions
- Anti-loop wake-up logic

## Mocks and Abstractions

To isolate the business logic, we mock hardware-dependent layers:

### Google Mock (GMock)

Component interfaces are mocked using Google Mock, providing:
- **Call Spying:** Track if hardware methods were called.
- **Stubbing:** Configure return values for GPIO states.

Mock files are located in `host_test/common/`:
- `mock_hal_gpio.hpp`
- `mock_hal_timer.hpp`

## Directory Structure

```
host_test/
├── common/                     # Shared mocks and utilities
├── test_floatswitch/           # FloatSwitch component tests
├── CMakeLists.txt              # Unified test configuration
├── coverage_common.cmake       # Shared coverage logic
└── README.md                   # This file
```

## Running Tests

### Prerequisites

- Linux host machine
- ESP-IDF environment set up
- `lcov` and `genhtml` (for coverage reporting)

### Running All Tests (Unified)

To run all tests and generate a coverage report:

1. **Configure and build the project**:
   ```bash
   cd host_test
   cmake -B build -S .
   cmake --build build --target build_all_tests
   ```

2. **Run the tests**:
   ```bash
   cd build
   ctest --output-on-failure
   ``` 

3. **Generate coverage report**:
   ```bash
   cmake --build . --target unified_coverage
   ```

The report will be available at `host_test/coverage/index.html`.

## CI/CD Integration

This project includes a GitHub Actions workflow (`.github/workflows/host_test.yml`) that automatically:

- Builds and runs all host tests on every push/PR to `main`.
- Generates and deploys the unified coverage report to GitHub Pages (on `main` pushes).
