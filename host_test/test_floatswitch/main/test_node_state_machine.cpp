#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "float_switch.hpp"
#include "mock_hal_gpio.hpp"
#include "mock_hal_timer.hpp"

using namespace floatswitch;
using ::testing::Return;
using ::testing::_;

class FloatSwitchTest : public ::testing::Test
{
protected:
    MockGpioHAL mock_gpio;
    MockTimerHAL mock_timer;
    
    FloatSwitch::Config cfg = {
        .gpio = GPIO_NUM_4,
        .normally_open = true,
        .debounce_time_us = 50000,
        .wakeup_on = IFloatSwitch::WakeupCondition::NEVER,
        .active_level = 0 // LOW
    };

    void SetUp() override {
        EXPECT_CALL(mock_gpio, config(_)).WillRepeatedly(Return(ESP_OK));
        EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(1));
        EXPECT_CALL(mock_timer, get_time_us()).WillRepeatedly(Return(0));
    }
};

TEST_F(FloatSwitchTest, InitAndDeinit)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    EXPECT_EQ(fs.init(), ESP_OK);
    EXPECT_EQ(fs.deinit(), ESP_OK);
}

TEST_F(FloatSwitchTest, TankFullNormallyOpen)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // NO switch: Tank full -> Contact OPEN (Electrical HIGH if active_level is LOW)
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, get_time_us()).WillRepeatedly(Return(100000));
    EXPECT_TRUE(fs.is_tank_full());

    // NO switch: Tank empty -> Contact CLOSED (Electrical LOW if active_level is LOW)
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillRepeatedly(Return(200000));
    EXPECT_FALSE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, DebounceGlitchRejection)
{
    cfg.debounce_time_us = 50000;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Start with HIGH (Tank full)
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(0));
    fs.is_tank_full();

    // Signal drops to LOW at t=10ms
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(10000));
    EXPECT_TRUE(fs.is_tank_full()); // Still FULL (not debounced yet)

    // Signal goes back to HIGH (glitch) at t=20ms
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(20000));
    EXPECT_TRUE(fs.is_tank_full()); // Still FULL

    // Signal stays HIGH at t=70ms
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(70000));
    EXPECT_TRUE(fs.is_tank_full()); // Confirmed FULL
}
