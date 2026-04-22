#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "float_switch.hpp"

static const char *TAG = "FloatSwitch";

// Production Constructor
FloatSwitch::FloatSwitch(const Config &cfg)
    : internal_gpio_hal_()
    , internal_timer_hal_()
    , internal_input_({cfg.gpio, cfg.debounce_time_us, cfg.active_level}, internal_gpio_hal_,
                      internal_timer_hal_)
    , input_(internal_input_) // Use the internal stack
    , normally_open_(cfg.normally_open)
    , wakeup_on_(cfg.wakeup_on)
{
}

// Test/DI Constructor
FloatSwitch::FloatSwitch(IBinaryInput &input, bool normally_open, WakeupCondition wakeup_on)
    : internal_gpio_hal_()
    , internal_timer_hal_()
    , internal_input_({GPIO_NUM_NC, 0, DebouncedInput::ActiveLevel::LOW}, internal_gpio_hal_,
                      internal_timer_hal_) // Dummy internal input
    , input_(input)                        // Use the injected input
    , normally_open_(normally_open)
    , wakeup_on_(wakeup_on)
{
}

esp_err_t FloatSwitch::init()
{
    ESP_LOGI(TAG, "Initializing FloatSwitch");

    // If we are using the internal input (production), we must initialize it.
    // If input_ points to internal_input_, initialize it.
    if (&input_ == &internal_input_) {
        ESP_RETURN_ON_ERROR(internal_input_.init(), TAG, "Failed to initialize internal input");
    }

    initialized_ = true;
    return ESP_OK;
}

esp_err_t FloatSwitch::deinit()
{
    ESP_LOGI(TAG, "Deinitializing FloatSwitch");
    initialized_ = false;
    return ESP_OK;
}

bool FloatSwitch::is_tank_full()
{
    if (!initialized_) {
        ESP_LOGE(TAG, "FloatSwitch not initialized");
        return false;
    }

    bool contact_closed = input_.is_active();

    if (normally_open_) {
        return !contact_closed;
    }
    else {
        return contact_closed;
    }
}

bool FloatSwitch::should_enable_wakeup()
{
    if (!initialized_) {
        ESP_LOGE(TAG, "FloatSwitch not initialized");
        return false;
    }

    switch (wakeup_on_) {
    case WakeupCondition::NEVER:
        return false;

    case WakeupCondition::WHEN_TANK_IS_EMPTY:
        return is_tank_full();

    case WakeupCondition::WHEN_TANK_IS_FULL:
        return !is_tank_full();

    default:
        return false;
    }
}
