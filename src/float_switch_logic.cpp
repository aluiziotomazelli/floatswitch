#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include "float_switch_logic.hpp"

namespace floatswitch {

static const char *TAG = "FloatSwitchLogic";

FloatSwitchLogic::FloatSwitchLogic(IBinaryInput &input, bool normally_open, WakeupCondition wakeup_on)
    : input_(input)
    , normally_open_(normally_open)
    , wakeup_on_(wakeup_on)
{
}

esp_err_t FloatSwitchLogic::init()
{
    initialized_ = true;
    return ESP_OK;
}

esp_err_t FloatSwitchLogic::deinit()
{
    initialized_ = false;
    return ESP_OK;
}

bool FloatSwitchLogic::is_tank_full()
{
    if (!initialized_) {
        ESP_LOGE(TAG, "Not initialized");
        return false;
    }

    bool contact_closed = input_.is_active();

    if (normally_open_) {
        // NO switch: Tank full if contact is OPEN
        return !contact_closed;
    }
    else {
        // NC switch: Tank full if contact is CLOSED
        return contact_closed;
    }
}

bool FloatSwitchLogic::should_enable_wakeup()
{
    if (!initialized_) {
        ESP_LOGE(TAG, "Not initialized");
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

} // namespace floatswitch
