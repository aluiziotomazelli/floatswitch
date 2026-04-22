#include "float_switch.hpp"
#include "esp_log.h"

namespace floatswitch {

static const char *TAG = "FloatSwitch";

FloatSwitch::FloatSwitch(const Config &cfg)
    : FloatSwitchLogic(internal_input_, cfg.normally_open, cfg.wakeup_on)
    , internal_gpio_hal_()
    , internal_timer_hal_()
    , internal_input_({cfg.gpio, cfg.debounce_time_us, cfg.active_level}, internal_gpio_hal_,
                      internal_timer_hal_)
{
}

esp_err_t FloatSwitch::init()
{
    ESP_LOGI(TAG, "Initializing production FloatSwitch stack");

    // 1. Initialize the internal input (which initializes the HAL)
    esp_err_t ret = internal_input_.init();
    if (ret != ESP_OK) {
        return ret;
    }

    // 2. Initialize the base logic
    return FloatSwitchLogic::init();
}

} // namespace floatswitch
