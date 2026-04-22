# Refactoring Plan: FloatSwitch Component

This document outlines the architectural and structural improvements for the `floatswitch` component to align with the project's strict HAL and SRP (Single Responsibility Principle) requirements.

## Final Architecture

The component is organized into a layered, namespaced structure that supports both automated bundling and manual Dependency Injection (DI).

### Layers
1.  **Hardware Abstraction (HAL)**: `IGpioHAL`, `ITimerHAL` (1:1 ESP-IDF wrappers).
2.  **Signal Conditioning**: `DebouncedInput` (Non-blocking timing-based debouncer).
3.  **Domain Logic**: `FloatSwitchLogic` (Tank interpretation and wakeup policy).
4.  **Production Bundle**: `FloatSwitch` (Convenience class that wires the logic with an internal HAL stack).

### Namespace
All symbols are enclosed in `namespace floatswitch` to prevent naming collisions with other components.

## Usage Patterns

### 1. Simple Production Use
Ideal for single-sensor setups where convenience is priority.
```cpp
floatswitch::FloatSwitch::Config cfg = { .gpio = GPIO_NUM_4 };
floatswitch::FloatSwitch sensor(cfg);
sensor.init();
```

### 2. Manual Dependency Injection (Shared HALs)
Ideal for multi-sensor setups to minimize resource redundancy.
```cpp
static floatswitch::GpioHAL shared_gpio;
static floatswitch::TimerHAL shared_timer;

floatswitch::DebouncedInput input1({GPIO_4, ...}, shared_gpio, shared_timer);
floatswitch::FloatSwitchLogic sensor1(input1, ...);
```

### 3. Unit Testing
Mocks are injected into the domain logic to verify behavior without hardware.
```cpp
MockBinaryInput mock_input;
FloatSwitchLogic sensor(mock_input, ...);
EXPECT_CALL(mock_input, is_active()).WillOnce(Return(true));
```

## Benefits
- **Zero Naming Conflicts**: Scoped within `floatswitch`.
- **High Testability**: 100% logic coverage on host machines.
- **Resource Efficient**: Supports shared HAL instances across multiple sensors.
- **Clean API**: Clear separation between wiring and business logic.
