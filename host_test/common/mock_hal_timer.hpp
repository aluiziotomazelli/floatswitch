#pragma once

#include "gmock/gmock.h"
#include "interfaces/i_hal_timer.hpp"

class MockTimerHAL : public ITimerHAL
{
public:
    MOCK_METHOD(int64_t, get_time_us, (), (const, override));
    MOCK_METHOD(void, delay_us, (uint32_t us), (const, override));
};
