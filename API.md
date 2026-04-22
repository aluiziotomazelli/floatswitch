# FloatSwitch Component API

This document provides the reference for the FloatSwitch component API.

---

## Core Types

### `enum class ActiveLevel`
Defines the electrical level that represents an "active" state, determined by the hardware wiring.

| Value | Description |
|-------|-------------|
| `LOW` | Signal is LOW when the switch contact is closed (requires Pull-Up). |
| `HIGH` | Signal is HIGH when the switch contact is closed (requires Pull-Down). |

### `enum class WakeupCondition`
Defines the logical condition under which a GPIO wake-up should be triggered.

| Value | Description |
|-------|-------------|
| `NEVER` | Never triggers wake-up. |
| `WHEN_TANK_IS_EMPTY` | Trigger wake-up only when the tank becomes empty. |
| `WHEN_TANK_IS_FULL` | Trigger wake-up only when the tank becomes full. |

### `struct Config`
Configuration parameters for the `FloatSwitch` component.

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `gpio` | `gpio_num_t` | N/A | ESP-IDF GPIO pin number. |
| `normally_open` | `bool` | `true` | `true` for NO, `false` for NC. |
| `debounce_time_us` | `uint32_t` | 50000 | Debounce delay in microseconds. |
| `active_level` | `ActiveLevel` | `LOW` | Electrical active level. |
| `wakeup_on` | `WakeupCondition`| `NEVER` | Condition for Deep Sleep wake-up. |

---

## Component Interface

### `class IFloatSwitch`
Abstract interface providing high-level tank state monitoring.

#### `esp_err_t init()`
Initializes the hardware and component state.
* **Returns**: `ESP_OK` on success.

#### `esp_err_t deinit()`
Deinitializes the hardware and resets GPIO states.
* **Returns**: `ESP_OK` on success.

#### `bool is_tank_full()`
Reads and processes the current hardware state to determine if the tank is full.
* **Returns**: `true` if the tank is full, `false` if empty.

#### `bool should_enable_wakeup()`
Logic to determine if the hardware wake-up trigger should be armed before entering low-power/sleep modes, preventing wake-up loops.
* **Returns**: `true` if wake-up should be enabled, `false` otherwise.
