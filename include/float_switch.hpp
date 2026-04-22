#pragma once

#include "interfaces/i_float_switch.hpp"
#include "interfaces/i_binary_input.hpp"
#include "debounced_input.hpp"
#include "hal_gpio.hpp"
#include "hal_timer.hpp"

/**
 * @file float_switch.hpp
 * @brief Implementation of the IFloatSwitch interface.
 */

class FloatSwitch : public IFloatSwitch
{
public:
    enum class WakeupCondition
    {
        NEVER,
        WHEN_TANK_IS_EMPTY,
        WHEN_TANK_IS_FULL,
    };

    /**
     * @brief Configuration for production use (handles its own HALs).
     */
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
     * Automatically creates internal HAL and DebouncedInput instances.
     * @param cfg Production configuration.
     */
    explicit FloatSwitch(const Config &cfg);

    /**
     * @brief Test/DI constructor.
     * Use this to inject a mock input for unit testing.
     * @param input Injected binary input.
     * @param normally_open Switch type.
     * @param wakeup_on Wakeup condition.
     */
    FloatSwitch(IBinaryInput &input, bool normally_open, WakeupCondition wakeup_on);

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
    // Internal HAL stack for production use
    GpioHAL internal_gpio_hal_;
    TimerHAL internal_timer_hal_;
    DebouncedInput internal_input_;

    // Domain configuration
    IBinaryInput &input_;
    bool normally_open_;
    WakeupCondition wakeup_on_;

    bool initialized_ = false;
};
