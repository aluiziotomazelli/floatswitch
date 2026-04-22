#include "debounced_input.hpp"

DebouncedInput::DebouncedInput(const Config &cfg, IGpioHAL &gpio_hal, ITimerHAL &timer_hal)
    : cfg_(cfg)
    , gpio_hal_(gpio_hal)
    , timer_hal_(timer_hal)
{
}

esp_err_t DebouncedInput::init()
{
    gpio_config_t gpio_cfg{};
    gpio_cfg.pin_bit_mask = (1ULL << cfg_.gpio);
    gpio_cfg.mode         = GPIO_MODE_INPUT;
    gpio_cfg.intr_type    = GPIO_INTR_DISABLE;

    if (cfg_.active_level == ActiveLevel::LOW) {
        gpio_cfg.pull_up_en   = GPIO_PULLUP_ENABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    else {
        gpio_cfg.pull_up_en   = GPIO_PULLUP_DISABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    esp_err_t ret = gpio_hal_.config(&gpio_cfg);
    if (ret == ESP_OK) {
        initialized_      = true;
        last_raw_level_   = gpio_hal_.get_level(cfg_.gpio);
        stable_level_     = last_raw_level_;
        last_change_us_ = timer_hal_.get_time_us();
    }
    return ret;
}

bool DebouncedInput::is_active()
{
    if (!initialized_) {
        return false;
    }

    bool current_raw = gpio_hal_.get_level(cfg_.gpio);
    int64_t now      = timer_hal_.get_time_us();

    if (current_raw != last_raw_level_) {
        last_change_us_ = now;
        last_raw_level_ = current_raw;
    }
    else {
        if ((now - last_change_us_) >= static_cast<int64_t>(cfg_.debounce_time_us)) {
            stable_level_ = current_raw;
        }
    }

    if (cfg_.active_level == ActiveLevel::LOW) {
        return !stable_level_;
    }
    else {
        return stable_level_;
    }
}
