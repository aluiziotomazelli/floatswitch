#pragma once

#include <cstdint>

namespace floatswitch {

/**
 * @file i_hal_timer.hpp
 * @brief Interface for system time services.
 */

/**
 * @interface ITimerHAL
 * @brief Interface for system time services.
 * @internal
 */
class ITimerHAL
{
public:
    virtual ~ITimerHAL() = default;

    /** @copydoc esp_timer_get_time() */
    virtual int64_t get_time_us() const = 0;

    /** @copydoc esp_rom_delay_us() */
    virtual void delay_us(uint32_t us) const = 0;
};

} // namespace floatswitch
