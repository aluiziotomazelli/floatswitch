#pragma once

#include "interfaces/i_binary_input.hpp"
#include "interfaces/i_hal_gpio.hpp"
#include "interfaces/i_hal_timer.hpp"
#include "driver/gpio.h"

/**
 * @file debounced_input.hpp
 * @brief Non-blocking debounced binary input implementation.
 */

class DebouncedInput : public IBinaryInput
{
public:
    enum class ActiveLevel
    {
        LOW,
        HIGH,
    };

    struct Config
    {
        gpio_num_t gpio;
        uint32_t debounce_time_us;
        ActiveLevel active_level;
    };

    DebouncedInput(const Config &cfg, IGpioHAL &gpio_hal, ITimerHAL &timer_hal);

    /**
     * @brief Initializes the underlying GPIO.
     * @return ESP_OK on success.
     */
    esp_err_t init();

    /** @copydoc IBinaryInput::is_active() */
    bool is_active() override;

private:
    Config cfg_;
    IGpioHAL &gpio_hal_;
    ITimerHAL &timer_hal_;

    bool last_raw_level_   = false;
    bool stable_level_     = false;
    int64_t last_change_us_ = 0;
    bool initialized_      = false;
};
