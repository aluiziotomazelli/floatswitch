# Refactoring Plan: FloatSwitch Component

This document outlines the architectural and structural improvements for the `floatswitch` component to align with the project's strict HAL and SRP (Single Responsibility Principle) requirements.

## Current Issues

1.  **Strict HAL Violation**: The component directly calls ESP-IDF GPIO and ROM APIs, making it impossible to run unit tests on a host (Linux) without hardware emulation.
2.  **SRP Violation**: The `FloatSwitch` class manages hardware configuration, implements debouncing logic, and performs domain-specific interpretation (tank state) in a single class.
3.  **Hardcoded Dependencies**: Lack of dependency injection prevents the use of mocks for testing.
4.  **Blocking Logic**: The current debouncing uses `esp_rom_delay_us`, which blocks execution and is hardware-dependent.

## Proposed Architecture

The component will be decomposed into three distinct layers to ensure separation of concerns and testability.

### Layer 1: Hardware Abstraction (HAL)
- **`IGpioHal`**: Interface defining 1:1 wrappers for ESP-IDF GPIO functions (`config`, `get_level`, `reset`).
- **`IClockHal`**: Interface for time-related functions (`delay_us`, `get_time`).

### Layer 2: Signal Conditioning
- **`DebouncedInput`**: A class responsible for providing a stable boolean state from a noisy input.
    - **Responsibility**: Polling/Interrupt management and debouncing algorithm.
    - **Dependencies**: Injected `IGpioHal` and `IClockHal`.

### Layer 3: Domain Logic
- **`FloatSwitch`**: The high-level component representing the physical sensor.
    - **Responsibility**: Interpreting the stable signal into domain states (`isTankFull`) and providing power management hints (`shouldEnableWakeup`).
    - **Dependencies**: Injected `IDebouncedInput` (interface).

## Implementation Steps

### Phase 1: Infrastructure
1.  Define interfaces in `include/interface/`:
    - `IGpioHal.hpp`
    - `IClockHal.hpp`
    - `IBinaryInput.hpp` (to be implemented by `DebouncedInput`)
2.  Implement ESP-IDF wrappers in `src/hal/`.

### Phase 2: Refactoring
1.  **Refactor Debouncing**: Extract the logic from `FloatSwitch` into a standalone `DebouncedInput` class.
2.  **Refactor FloatSwitch**: Update the class to accept an `IBinaryInput` interface via constructor injection.
3.  **Remove Direct Includes**: Eliminate all `driver/gpio.h`, `esp_rom_sys.h`, and other ESP-IDF headers from the domain logic files.

### Phase 3: Validation
1.  **Mocking**: Create Google Mock implementations for all interfaces in `host_test_common/`.
2.  **Host Tests**: Update `host_test/test_floatswitch/` to perform exhaustive unit testing of the domain logic and debouncing timing on the host.

## Impact Analysis
- **Breaking Change**: The constructor of `FloatSwitch` and its initialization flow will change significantly.
- **Improved Testability**: 100% code coverage can be achieved on host machines without ESP-32 hardware.
- **Portability**: The domain logic can be reused on any platform by providing a different HAL implementation.
