#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

namespace floatswitch {

/**
 * @file i_fs_hal_gpio.hpp
 * @brief Interface for GPIO hardware abstraction.
 */

/**
 * @interface IGpioHAL
 * @brief 1:1 wrapper for ESP-IDF GPIO functions.
 */
class IGpioHAL
{
public:
    virtual ~IGpioHAL() = default;

    /** @copydoc gpio_config() */
    virtual esp_err_t config(const gpio_config_t *p_cfg) = 0;

    /** @copydoc gpio_reset_pin() */
    virtual esp_err_t reset_pin(gpio_num_t gpio_num) = 0;

    /** @copydoc gpio_get_level() */
    virtual int get_level(gpio_num_t gpio_num) const = 0;
};

} // namespace floatswitch
