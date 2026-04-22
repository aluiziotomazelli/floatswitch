#pragma once

#include "float_switch_logic.hpp"
#include "debounced_input.hpp"
#include "hal_gpio.hpp"
#include "hal_timer.hpp"

namespace floatswitch {

/**
 * @file float_switch.hpp
 * @brief Production bundle for the FloatSwitch component.
 */

class FloatSwitch : public FloatSwitchLogic
{
public:
    struct Config
    {
        gpio_num_t gpio;
        DebouncedInput::ActiveLevel active_level = DebouncedInput::ActiveLevel::LOW;
        uint32_t debounce_time_us                = 50000;
        bool normally_open                       = true;
        WakeupCondition wakeup_on                = WakeupCondition::NEVER;
    };

    /**
     * @brief Production constructor.
     * Owns and initializes the entire hardware stack.
     */
    explicit FloatSwitch(const Config &cfg);

    ~FloatSwitch() override = default;

    /** @copydoc FloatSwitchLogic::init() */
    esp_err_t init() override;

private:
    GpioHAL internal_gpio_hal_;
    TimerHAL internal_timer_hal_;
    DebouncedInput internal_input_;
};

} // namespace floatswitch
