#pragma once

#include "interfaces/i_hal_timer.hpp"

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
