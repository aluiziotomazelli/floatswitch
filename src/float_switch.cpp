#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "float_switch.hpp"

namespace floatswitch {

static const char *TAG = "FloatSwitch";

FloatSwitch::FloatSwitch(const Config &cfg, IGpioHAL &gpio_hal, ITimerHAL &timer_hal)
    : cfg_(cfg)
    , gpio_hal_(gpio_hal)
    , timer_hal_(timer_hal)
{
}

esp_err_t FloatSwitch::init()
{
    ESP_LOGI(TAG, "Initializing FloatSwitch on GPIO %d", cfg_.gpio);

    gpio_config_t gpio_cfg{};
    gpio_cfg.pin_bit_mask = (1ULL << cfg_.gpio);
    gpio_cfg.mode         = GPIO_MODE_INPUT;
    gpio_cfg.intr_type    = GPIO_INTR_DISABLE;

    if (cfg_.active_level == 0) { // LOW
        gpio_cfg.pull_up_en   = GPIO_PULLUP_ENABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    else { // HIGH
        gpio_cfg.pull_up_en   = GPIO_PULLUP_DISABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    esp_err_t ret = gpio_hal_.config(&gpio_cfg);
    if (ret == ESP_OK) {
        initialized_    = true;
        last_raw_level_ = gpio_hal_.get_level(cfg_.gpio);
        stable_level_   = last_raw_level_;
        last_change_us_ = timer_hal_.get_time_us();
    }
    return ret;
}

esp_err_t FloatSwitch::deinit()
{
    initialized_ = false;
    return gpio_hal_.reset_pin(cfg_.gpio);
}

bool FloatSwitch::is_contact_closed()
{
    if (!initialized_) return false;

    bool current_raw = gpio_hal_.get_level(cfg_.gpio);
    int64_t now      = timer_hal_.get_time_us();

    if (current_raw != last_raw_level_) {
        last_change_us_ = now;
        last_raw_level_ = current_raw;
    }
    else if ((now - last_change_us_) >= static_cast<int64_t>(cfg_.debounce_time_us)) {
        stable_level_ = current_raw;
    }

    // Translate electrical level to contact state
    if (cfg_.active_level == 0) { // LOW
        return !stable_level_;
    } else { // HIGH
        return stable_level_;
    }
}

bool FloatSwitch::is_tank_full()
{
    bool contact_closed = is_contact_closed();

    if (cfg_.normally_open) {
        return !contact_closed; // NO: Full when open
    }
    else {
        return contact_closed; // NC: Full when closed
    }
}

bool FloatSwitch::should_enable_wakeup()
{
    if (!initialized_) return false;

    switch (cfg_.wakeup_on) {
    case WakeupCondition::WHEN_TANK_IS_EMPTY:
        return is_tank_full();
    case WakeupCondition::WHEN_TANK_IS_FULL:
        return !is_tank_full();
    default:
        return false;
    }
}

} // namespace floatswitch
