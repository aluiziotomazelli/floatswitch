#include "esp_log.h"
#include "float_switch.hpp"
#include "hal_gpio.hpp"
#include "hal_timer.hpp"

extern "C" void app_main(void)
{
    ESP_LOGI("main", "FloatSwitch - Simple and Clean DI");

    // 1. Declare the tools (stateless HALs)
    static floatswitch::GpioHAL gpio;
    static floatswitch::TimerHAL timer;

    // 2. Configure and create the sensor
    floatswitch::FloatSwitch::Config cfg = {
        .gpio             = GPIO_NUM_4,
        .normally_open    = true,
        .debounce_time_us = 50000,
        .wakeup_on        = floatswitch::IFloatSwitch::WakeupCondition::WHEN_TANK_IS_EMPTY,
        .active_level     = 0 // LOW
    };

    floatswitch::FloatSwitch sensor(cfg, gpio, timer);

    if (sensor.init() == ESP_OK) {
        bool full = sensor.is_tank_full();
        ESP_LOGI("main", "Tank status: %s", full ? "FULL" : "EMPTY");
        sensor.deinit();
    }
}
