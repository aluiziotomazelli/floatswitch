#include "esp_log.h"
#include "float_switch_logic.hpp"
#include "debounced_input.hpp"
#include "hal_gpio.hpp"
#include "hal_timer.hpp"

extern "C" void app_main(void)
{
    ESP_LOGI("main", "Testing FloatSwitch component with Manual DI and Namespaces");

    // 1. Single HAL instances shared by multiple sensors
    static floatswitch::GpioHAL gpio_hal;
    static floatswitch::TimerHAL timer_hal;

    // 2. Sensor 1 configuration
    floatswitch::DebouncedInput::Config cfg1 = {
        .gpio              = GPIO_NUM_4,
        .debounce_time_us  = 50000,
        .active_level      = floatswitch::DebouncedInput::ActiveLevel::LOW
    };
    floatswitch::DebouncedInput input1(cfg1, gpio_hal, timer_hal);
    input1.init();

    floatswitch::FloatSwitchLogic sensor1(input1, true, floatswitch::IFloatSwitch::WakeupCondition::WHEN_TANK_IS_EMPTY);
    sensor1.init();

    // 3. Sensor 2 configuration (Sharing the same HALs)
    floatswitch::DebouncedInput::Config cfg2 = {
        .gpio              = GPIO_NUM_5,
        .debounce_time_us  = 50000,
        .active_level      = floatswitch::DebouncedInput::ActiveLevel::LOW
    };
    floatswitch::DebouncedInput input2(cfg2, gpio_hal, timer_hal);
    input2.init();

    floatswitch::FloatSwitchLogic sensor2(input2, true, floatswitch::IFloatSwitch::WakeupCondition::WHEN_TANK_IS_FULL);
    sensor2.init();

    // Verify
    bool full1 = sensor1.is_tank_full();
    bool full2 = sensor2.is_tank_full();
    ESP_LOGI("main", "Sensor 1 (GPIO 4): %s", full1 ? "FULL" : "EMPTY");
    ESP_LOGI("main", "Sensor 2 (GPIO 5): %s", full2 ? "FULL" : "EMPTY");

    sensor1.deinit();
    sensor2.deinit();
}
