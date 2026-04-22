#pragma once

#include "esp_err.h"

namespace floatswitch {

/**
 * @file i_float_switch.hpp
 * @brief Interface for the FloatSwitch component.
 */

/**
 * @interface IFloatSwitch
 * @brief Provides high-level abstraction for a tank float switch sensor.
 */
class IFloatSwitch
{
public:
    virtual ~IFloatSwitch() = default;

    /**
     * @brief Initializes the component.
     * @return ESP_OK on success.
     */
    virtual esp_err_t init() = 0;

    /**
     * @brief Deinitializes the component.
     * @return ESP_OK on success.
     */
    virtual esp_err_t deinit() = 0;

    /**
     * @brief Determines the logical state of the tank.
     * @return True if the tank is full, false if empty.
     */
    virtual bool is_tank_full() = 0;

    /**
     * @brief Checks if a wake-up trigger should be armed.
     * @return True if the GPIO wake-up source should be enabled.
     */
    virtual bool should_enable_wakeup() = 0;
};

} // namespace floatswitch
