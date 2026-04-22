#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "float_switch.hpp"
#include "mock_hal_gpio.hpp"
#include "mock_hal_timer.hpp"

using namespace floatswitch;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::Return;

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
        .wakeup_on = WakeupCondition::NEVER};

    void SetUp() override
    {
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
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);

    EXPECT_TRUE(fs.is_tank_full());

    // NO switch: Tank empty -> Contact CLOSED
    // With ActiveLevel::LOW, CLOSED contact means electrical LOW (0)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);

    EXPECT_FALSE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, DebounceMajorityVote)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Simulate a noisy signal: 6 samples HIGH, 4 samples LOW
    // Majority (6/10) is HIGH -> Contact OPEN -> Tank FULL (for NO)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio))
        .Times(DEBOUNCE_SAMPLES)
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

    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);

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

TEST_F(FloatSwitchTest, LogicActiveHighNormallyOpen)
{
    // Test Scenario C: NO + Active HIGH
    cfg.normally_open = true;
    cfg.active_level = ActiveLevel::HIGH;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Empty: Float down -> Contact Closed -> Signal 1 (HIGH)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_FALSE(fs.is_tank_full());

    // Full: Float up -> Contact Open -> Signal 0 (LOW)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_TRUE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, LogicActiveHighNormallyClosed)
{
    // Test Scenario D: NC + Active HIGH
    cfg.normally_open = false;
    cfg.active_level  = ActiveLevel::HIGH;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Empty: Float down -> Contact Open -> Signal 0 (pull-down)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_FALSE(fs.is_tank_full());

    // Full: Float up -> Contact Closed -> Signal 1 (VCC)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_TRUE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, LogicActiveLowNormallyOpen)
{
    // Test Scenario A: NO + Active LOW
    cfg.normally_open = true;
    cfg.active_level  = ActiveLevel::LOW;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Empty: Float down -> Contact Closed -> Signal 0 (GND)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_FALSE(fs.is_tank_full());

    // Full: Float up -> Contact Open -> Signal 1 (pull-up)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_TRUE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, LogicActiveLowNormallyClosed)
{
    // Test Scenario B: NC + Active LOW
    cfg.normally_open = false;
    cfg.active_level  = ActiveLevel::LOW;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Empty: Float down -> Contact Open -> Signal 1 (pull-up)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_FALSE(fs.is_tank_full());

    // Full: Float up -> Contact Closed -> Signal 0 (GND)
    EXPECT_CALL(mock_gpio, get_level(cfg.gpio)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_TRUE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, WakeupAntiLoop)
{
    // Configure to wakeup when FULL
    cfg.wakeup_on = WakeupCondition::WHEN_TANK_IS_FULL;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    fs.init();

    // Current: EMPTY (Tank Empty -> Signal 0 for NO/LOW) -> Arm Wakeup
    EXPECT_CALL(mock_gpio, get_level(_)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(0));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_TRUE(fs.should_enable_wakeup());

    // Current: FULL (Tank Full -> Signal 1 for NO/LOW) -> Do not arm (avoid loop)
    EXPECT_CALL(mock_gpio, get_level(_)).Times(DEBOUNCE_SAMPLES).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_timer, delay_us(_)).Times(DEBOUNCE_SAMPLES);
    EXPECT_FALSE(fs.should_enable_wakeup());
}

TEST_F(FloatSwitchTest, InitFailure)
{
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    // Force config to fail
    EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_ERR_INVALID_ARG));

    EXPECT_EQ(fs.init(), ESP_ERR_INVALID_ARG);

    // If not initialized, should_enable_wakeup must return false
    EXPECT_FALSE(fs.should_enable_wakeup());
}

TEST_F(FloatSwitchTest, IsTankFullUninitialized)
{
    // NO switch (default)
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    // Not calling fs.init()

    // is_contact_closed() returns false when not initialized.
    // For NO: is_tank_full() = !is_contact_closed() = true
    EXPECT_TRUE(fs.is_tank_full());

    // For NC switch
    cfg.normally_open = false;
    FloatSwitch fs_nc(cfg, mock_gpio, mock_timer);
    // For NC: is_tank_full() = is_contact_closed() = false
    EXPECT_FALSE(fs_nc.is_tank_full());
}

TEST_F(FloatSwitchTest, ShouldEnableWakeupUninitialized)
{
    cfg.wakeup_on = WakeupCondition::WHEN_TANK_IS_EMPTY;
    FloatSwitch fs(cfg, mock_gpio, mock_timer);
    // Not calling fs.init()

    // should_enable_wakeup() returns false when not initialized
    EXPECT_FALSE(fs.should_enable_wakeup());
}

