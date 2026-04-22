#include "hal_timer.hpp"
#include "esp_timer.h"
#include "esp_rom_sys.h"

namespace floatswitch {

int64_t TimerHAL::get_time_us() const
{
    return esp_timer_get_time();
}

void TimerHAL::delay_us(uint32_t us) const
{
    esp_rom_delay_us(us);
}

} // namespace floatswitch
