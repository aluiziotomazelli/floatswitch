// include/floatswitch_types.hpp
#pragma once

#include <cstdint>
#include "driver/gpio.h"

namespace floatswitch {

constexpr uint8_t DEBOUNCE_SAMPLES = 10;

/**
 * @brief Defines the logical condition under which a GPIO wake-up should be
 * triggered. This abstracts the electrical signal (LOW/HIGH) into a desired
 * application behavior.
 */
enum class WakeupCondition
{
    NEVER,
    WHEN_TANK_IS_EMPTY,
    WHEN_TANK_IS_FULL,
};
/**
 * @brief Defines the electrical level that represents an "active" state.
 * This is determined by the hardware wiring (e.g., pull-up vs. pull-down
 * resistor).
 */
enum class ActiveLevel
{
    LOW,  ///< The signal is LOW when the switch contact is closed.
    HIGH, ///< The signal is HIGH when the switch contact is closed.
};
/**
 * @brief Configuration for the float switch.
 */
struct Config
{
    gpio_num_t gpio;
    bool normally_open = true;
    uint32_t debounce_time_us = 50000;
    ActiveLevel active_level = ActiveLevel::LOW;
    WakeupCondition wakeup_on = WakeupCondition::NEVER;
};
} // namespace floatswitch