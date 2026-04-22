#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "float_switch.hpp"
#include "mock_hal_gpio.hpp"
#include "mock_hal_timer.hpp"

using namespace floatswitch;
using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Exactly;

class FloatSwitchTest : public ::testing::Test
{
protected:
    MockGpioHAL mock_gpio;
    MockTimerHAL mock_timer;
    
    Config cfg = {
        .gpio = GPIO_NUM_4,
        .normally_open = true,
        .debounce_time_us = 50000,
        .active_level = ActiveLevel::LOW,
        .wakeup_on = WakeupCondition::NEVER
    };

    void SetUp() override {
        ON_CALL(mock_gpio, config(_)).WillByDefault(Return(ESP_OK));
        ON_CALL(mock_gpio, reset_pin(_)).WillByDefault(Return(ESP_OK));
    }
};

TEST_F(FloatSwitchTest, InitAndDeinit)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    EXPECT_CALL(mock_gpio, reset_pin(cfg.gpio)).Times(AtLeast(1));
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
    
    EXPECT_EQ(fs.init(), ESP_OK);
    EXPECT_EQ(fs.deinit(), ESP_OK);
}

TEST_F(FloatSwitchTest, TankFullNormallyOpen)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // NO switch: Tank full -> Contact OPEN
    // With ActiveLevel::LOW, OPEN contact means electrical HIGH (1)
    // We expect 10 samples and 10 delays
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(10).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(10);
    
    EXPECT_TRUE(fs.is_tank_full());

    // NO switch: Tank empty -> Contact CLOSED
    // With ActiveLevel::LOW, CLOSED contact means electrical LOW (0)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(10).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(10);
    
    EXPECT_FALSE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, DebounceMajorityVote)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Simulate a noisy signal: 6 samples HIGH, 4 samples LOW
    // Majority (6/10) is HIGH -> Contact OPEN -> Tank FULL (for NO)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    
    EXPECT_CALL(mock_timer, delay_us(_)).Times(10);
    
    EXPECT_TRUE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, WakeupLogic)
{
    cfg.wakeup_on = WakeupCondition::WHEN_TANK_IS_EMPTY;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // If tank is FULL, we SHOULD enable wakeup
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(1)); // Tank Full
    EXPECT_TRUE(fs.should_enable_wakeup());

    // If tank is EMPTY, we SHOULD NOT enable wakeup
    EXPECT_CALL(mock_gpio, get_level(_)).WillRepeatedly(Return(0)); // Tank Empty
    EXPECT_FALSE(fs.should_enable_wakeup());
}
