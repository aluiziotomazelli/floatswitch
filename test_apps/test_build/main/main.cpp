#include "esp_log.h"
#include "float_switch.hpp"

extern "C" void app_main(void)
{
    ESP_LOGI("main", "Testing FloatSwitch component with simplified architecture");

    // Simplified usage: The component handles its own internal stack
    FloatSwitch::Config fs_cfg = {
        .gpio              = GPIO_NUM_4,
        .active_level      = DebouncedInput::ActiveLevel::LOW,
        .debounce_time_us  = 50000,
        .normally_open     = true,
        .wakeup_on         = FloatSwitch::WakeupCondition::WHEN_TANK_IS_EMPTY
    };

    FloatSwitch sensor(fs_cfg);
    
    if (sensor.init() == ESP_OK) {
        bool full = sensor.is_tank_full();
        ESP_LOGI("main", "Tank is %s", full ? "FULL" : "EMPTY");
        sensor.deinit();
    }
}
