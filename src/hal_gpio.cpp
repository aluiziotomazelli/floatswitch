#include "hal_gpio.hpp"
#include "driver/gpio.h"

namespace floatswitch {

esp_err_t GpioHAL::config(const gpio_config_t *p_cfg)
{
    return gpio_config(p_cfg);
}

esp_err_t GpioHAL::reset_pin(gpio_num_t gpio_num)
{
    return gpio_reset_pin(gpio_num);
}

int GpioHAL::get_level(gpio_num_t gpio_num) const
{
    return gpio_get_level(gpio_num);
}

} // namespace floatswitch
