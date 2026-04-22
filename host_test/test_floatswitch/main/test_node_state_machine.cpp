#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "float_switch.hpp"
#include "mock_binary_input.hpp"

using ::testing::Return;

class FloatSwitchTest : public ::testing::Test
{
protected:
    MockBinaryInput mock_input;
};

TEST_F(FloatSwitchTest, InitDeinit)
{
    FloatSwitch fs(mock_input, true, FloatSwitch::WakeupCondition::NEVER);
    EXPECT_EQ(fs.init(), ESP_OK);
    EXPECT_EQ(fs.deinit(), ESP_OK);
}

TEST_F(FloatSwitchTest, TankFullNormallyOpen)
{
    FloatSwitch fs(mock_input, true, FloatSwitch::WakeupCondition::NEVER);
    fs.init();

    // NO switch: Tank full -> Contact OPEN (is_active = false)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(false));
    EXPECT_TRUE(fs.is_tank_full());

    // NO switch: Tank empty -> Contact CLOSED (is_active = true)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(true));
    EXPECT_FALSE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, TankFullNormallyClosed)
{
    FloatSwitch fs(mock_input, false, FloatSwitch::WakeupCondition::NEVER);
    fs.init();

    // NC switch: Tank full -> Contact CLOSED (is_active = true)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(true));
    EXPECT_TRUE(fs.is_tank_full());

    // NC switch: Tank empty -> Contact OPEN (is_active = false)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(false));
    EXPECT_FALSE(fs.is_tank_full());
}

TEST_F(FloatSwitchTest, WakeupLogicWhenTankIsEmpty)
{
    FloatSwitch fs(mock_input, true, FloatSwitch::WakeupCondition::WHEN_TANK_IS_EMPTY);
    fs.init();

    // If tank is FULL, we SHOULD enable wakeup (to catch when it becomes empty)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(false)); // Tank full
    EXPECT_TRUE(fs.should_enable_wakeup());

    // If tank is EMPTY, we SHOULD NOT enable wakeup (to avoid loop)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(true)); // Tank empty
    EXPECT_FALSE(fs.should_enable_wakeup());
}

TEST_F(FloatSwitchTest, WakeupLogicWhenTankIsFull)
{
    FloatSwitch fs(mock_input, true, FloatSwitch::WakeupCondition::WHEN_TANK_IS_FULL);
    fs.init();

    // If tank is EMPTY, we SHOULD enable wakeup (to catch when it becomes full)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(true)); // Tank empty
    EXPECT_TRUE(fs.should_enable_wakeup());

    // If tank is FULL, we SHOULD NOT enable wakeup (to avoid loop)
    EXPECT_CALL(mock_input, is_active()).WillOnce(Return(false)); // Tank full
    EXPECT_FALSE(fs.should_enable_wakeup());
}
