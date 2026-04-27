#pragma once

#include "interfaces/i_fs_hal_gpio.hpp"

namespace floatswitch {

/**
 * @file hal_gpio.hpp
 * @brief Thin wrapper for ESP-IDF GPIO functions.
 */

class GpioHAL : public IGpioHAL
{
public:
    /** @copydoc IGpioHAL::config() */
    esp_err_t config(const gpio_config_t *p_cfg) override;

    /** @copydoc IGpioHAL::reset_pin() */
    esp_err_t reset_pin(gpio_num_t gpio_num) override;

    /** @copydoc IGpioHAL::get_level() */
    int get_level(gpio_num_t gpio_num) const override;
};

} // namespace floatswitch
