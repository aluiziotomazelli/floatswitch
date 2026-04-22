#pragma once

#include "interfaces/i_hal_gpio.hpp"
#include "interfaces/i_hal_timer.hpp"
#include "interfaces/i_float_switch.hpp"

namespace floatswitch {

/**
 * @file float_switch.hpp
 * @brief Single class for FloatSwitch handling logic and debouncing.
 */
class FloatSwitch : public IFloatSwitch
{
public:
    /**
     * @brief Configuration for the float switch.
     */
    struct Config
    {
        gpio_num_t gpio;
        bool normally_open        = true;
        uint32_t debounce_time_us = 50000;
        WakeupCondition wakeup_on = WakeupCondition::NEVER;
        int active_level          = 0; // 0 for LOW, 1 for HIGH
    };

    /**
     * @brief Constructor with explicit Dependency Injection.
     * @param cfg Configuration.
     * @param gpio_hal Hardware abstraction for GPIO.
     * @param timer_hal Hardware abstraction for Timer.
     */
    FloatSwitch(const Config &cfg, IGpioHAL &gpio_hal, ITimerHAL &timer_hal);

    ~FloatSwitch() override = default;

    /** @copydoc IFloatSwitch::init() */
    esp_err_t init() override;

    /** @copydoc IFloatSwitch::deinit() */
    esp_err_t deinit() override;

    /** @copydoc IFloatSwitch::is_tank_full() */
    bool is_tank_full() override;

    /** @copydoc IFloatSwitch::should_enable_wakeup() */
    bool should_enable_wakeup() override;

private:
    Config cfg_;
    IGpioHAL &gpio_hal_;
    ITimerHAL &timer_hal_;

    bool initialized_      = false;
    bool last_raw_level_   = false;
    bool stable_level_     = false;
    int64_t last_change_us_ = 0;

    /**
     * @brief Internal debouncing logic (non-blocking).
     * @return Current stable electrical state.
     */
    bool is_contact_closed();
};

} // namespace floatswitch
