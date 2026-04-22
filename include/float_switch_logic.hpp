#pragma once

#include "interfaces/i_float_switch.hpp"
#include "interfaces/i_binary_input.hpp"

namespace floatswitch {

/**
 * @file float_switch_logic.hpp
 * @brief Pure domain logic for the float switch.
 */

class FloatSwitchLogic : public IFloatSwitch
{
public:
    FloatSwitchLogic(IBinaryInput &input, bool normally_open, WakeupCondition wakeup_on);
    ~FloatSwitchLogic() override = default;

    /** @copydoc IFloatSwitch::init() */
    esp_err_t init() override;

    /** @copydoc IFloatSwitch::deinit() */
    esp_err_t deinit() override;

    /** @copydoc IFloatSwitch::is_tank_full() */
    bool is_tank_full() override;

    /** @copydoc IFloatSwitch::should_enable_wakeup() */
    bool should_enable_wakeup() override;

protected:
    IBinaryInput &input_;
    bool normally_open_;
    WakeupCondition wakeup_on_;
    bool initialized_ = false;
};

} // namespace floatswitch
