# FloatSwitch Component

[![ESP-IDF Build](https://github.com/aluiziotomazelli/floatswitch/actions/workflows/build.yml/badge.svg)](https://github.com/aluiziotomazelli/floatswitch/actions/workflows/build.yml)
[![Host Tests](https://github.com/aluiziotomazelli/floatswitch/actions/workflows/host_test.yml/badge.svg)](https://github.com/aluiziotomazelli/floatswitch/actions/workflows/host_test.yml)
[![Coverage](https://img.shields.io/badge/coverage-95%25-orange)](https://aluiziotomazelli.github.io/floatswitch/index.html)

The `FloatSwitch` component provides a robust abstraction for float level sensors. It translates electrical GPIO signals into meaningful tank states (Full/Empty) while handling hardware debouncing and power-efficient wake-up logic for Deep Sleep cycles.

## Logic & Architecture

The component operates by decoupling the physical wiring from the application logic. It uses a majority-vote sampling algorithm to ensure that electrical noise doesn't trigger false state changes.

### Key Enumerations (`hpp`)

To provide flexibility for different hardware setups, the component uses three main enums:

1.  **`ActiveLevel`**: Defines the electrical state when the switch is physically closed.
    * `LOW`: The contact pulls the signal to GND (requires internal Pull-Up).
    * `HIGH`: The contact pulls the signal to VCC (requires internal Pull-Down).

2.  **`WakeupCondition`**: Defines when the system should wake up from Deep Sleep.
    * `NEVER`: Never Wakeup.
    * `WHEN_TANK_IS_EMPTY`: Wake up only when the water level drops.
    * `WHEN_TANK_IS_FULL`: Wake up only when the water level rises.

3.  **`normally_open` (Boolean Logic)**: 
    * If `true` (NO): A closed contact means the tank is **Full**.
    * If `false` (NC): A closed contact means the tank is **Empty**.



## Anti-Loop Wakeup Logic

One of the most critical features is the `shouldEnableWakeup()` method. It prevents the ESP32 from entering a "Wakeup Loop" (where the device wakes up, sees the trigger condition is already met, goes to sleep, and immediately wakes up again).

* **Logic**: It compares the **current stable state** with the **desired wakeup condition**.
* **Example**: If configured to wake up `WHEN_TANK_IS_EMPTY`, the function will only return `true` if the tank is currently `FULL`. If the tank is already empty, it prevents arming the trigger to save power and prevent useless wake cycles.

## Implementation Example

```cpp
#include "float_switch.hpp"

// Configuration for a sensor that pulls to GND when closed (NO)
// Note: GPIO_NUM_4 is an ESP-IDF specific type.
FloatSwitch::Config cfg = {
    .gpio = GPIO_NUM_4,
    .normally_open = true,                          // Normally Open
    .active_level = FloatSwitch::ActiveLevel::LOW,  // Pulls to GND
    .wakeup_on = FloatSwitch::WakeupCondition::WHEN_TANK_IS_EMPTY
};

FloatSwitch sensor(cfg);

void app_main() {
    if (sensor.init() == ESP_OK) {
        if (sensor.isTankFull()) {
            // Business logic for full tank
        }
        
        // Check if we should arm the wake-up pin before Deep Sleep
        bool arm_gpio = sensor.shouldEnableWakeup();
    }
}
```

## Unit Testing

This component includes a comprehensive suite of host-based unit tests to verify electrical logic, timing accuracy, and power management rules. 

For instructions on how to run the tests, see [host_test/README.md](host_test/README.md).