#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "debounced_input.hpp"
#include "mock_hal_gpio.hpp"
#include "mock_hal_timer.hpp"

using namespace floatswitch;
using ::testing::Return;
using ::testing::_;

class DebouncedInputTest : public ::testing::Test
{
protected:
    MockGpioHAL mock_gpio;
    MockTimerHAL mock_timer;

    const gpio_num_t TEST_GPIO = GPIO_NUM_4;
    const uint32_t DEBOUNCE_US = 50000; // 50ms

    DebouncedInput::Config cfg = {
        .gpio = TEST_GPIO,
        .debounce_time_us = DEBOUNCE_US,
        .active_level = DebouncedInput::ActiveLevel::LOW
    };

    void SetUp() override {
        // Default behavior for init
        EXPECT_CALL(mock_gpio, config(_)).WillOnce(Return(ESP_OK));
        EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(1)); // Initial HIGH (Inactive)
        EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(0));
    }
};

TEST_F(DebouncedInputTest, InitialState)
{
    DebouncedInput input(cfg, mock_gpio, mock_timer);
    input.init();
    EXPECT_FALSE(input.is_active());
}

TEST_F(DebouncedInputTest, DebounceLogic)
{
    DebouncedInput input(cfg, mock_gpio, mock_timer);
    input.init();

    // Signal goes LOW (Active) at t=100
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100));
    EXPECT_FALSE(input.is_active()); // Still inactive because it hasn't been DEBOUNCE_US yet

    // Signal stays LOW at t=100 + DEBOUNCE_US / 2
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100 + DEBOUNCE_US / 2));
    EXPECT_FALSE(input.is_active());

    // Signal stays LOW at t=100 + DEBOUNCE_US
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100 + DEBOUNCE_US));
    EXPECT_TRUE(input.is_active()); // Finally active!
}

TEST_F(DebouncedInputTest, GlitchRejection)
{
    DebouncedInput input(cfg, mock_gpio, mock_timer);
    input.init();

    // Signal goes LOW at t=100
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(0));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100));
    input.is_active();

    // Signal goes HIGH again (Glitch) at t=100 + DEBOUNCE_US / 2
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(1));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100 + DEBOUNCE_US / 2));
    EXPECT_FALSE(input.is_active());

    // Timer reaches t=100 + DEBOUNCE_US, but signal was reset
    EXPECT_CALL(mock_gpio, get_level(TEST_GPIO)).WillOnce(Return(1));
    EXPECT_CALL(mock_timer, get_time_us()).WillOnce(Return(100 + DEBOUNCE_US));
    EXPECT_FALSE(input.is_active());
}
