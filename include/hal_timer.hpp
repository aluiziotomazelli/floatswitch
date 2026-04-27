#pragma once

#include "interfaces/i_fs_hal_timer.hpp"

namespace floatswitch {

/**
 * @file hal_timer.hpp
 * @brief Thin wrapper for ESP-IDF timer functions.
 */

class TimerHAL : public ITimerHAL
{
public:
    /** @copydoc ITimerHAL::get_time_us() */
    int64_t get_time_us() const override;

    /** @copydoc ITimerHAL::delay_us() */
    void delay_us(uint32_t us) const override;
};

} // namespace floatswitch
