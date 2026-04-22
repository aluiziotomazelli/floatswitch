#pragma once

#include "gmock/gmock.h"
#include "interfaces/i_hal_gpio.hpp"

class MockGpioHAL : public IGpioHAL
{
public:
    MOCK_METHOD(esp_err_t, config, (const gpio_config_t *p_cfg), (override));
    MOCK_METHOD(esp_err_t, reset_pin, (gpio_num_t gpio_num), (override));
    MOCK_METHOD(int, get_level, (gpio_num_t gpio_num), (const, override));
};
