#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "float_switch.hpp"
#include "esp_log.h"

namespace floatswitch {

static const char* TAG = "FloatSwitch";

FloatSwitch::FloatSwitch(const Config& cfg, IGpioHAL& gpio_hal, ITimerHAL& timer_hal)
    : cfg_(cfg)
    , gpio_hal_(gpio_hal)
    , timer_hal_(timer_hal)
{
}

esp_err_t FloatSwitch::init()
{
    ESP_LOGI(TAG, "Initializing FloatSwitch on GPIO %d", cfg_.gpio);

    // Reset GPIO
    gpio_hal_.reset_pin(cfg_.gpio);

    // Set GPIO as input
    gpio_config_t gpio_cfg{};
    gpio_cfg.pin_bit_mask = (1ULL << cfg_.gpio);
    gpio_cfg.mode = GPIO_MODE_INPUT;
    gpio_cfg.intr_type = GPIO_INTR_DISABLE; // Interrupts are handled by the deep sleep wakeup controller.

    // The internal pull resistor is configured to oppose the active level.
    // This ensures that the pin is in a defined state when the switch contact is
    // open.
    // - If active_level is LOW (contact pulls to GND), we need a pull-up resistor.
    // - If active_level is HIGH (contact pulls to VCC), we need a pull-down
    // resistor.
    if (cfg_.active_level == floatswitch::ActiveLevel::LOW) {
        gpio_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    else { // ActiveLevel::HIGH
        gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    esp_err_t ret = gpio_hal_.config(&gpio_cfg);
    if (ret == ESP_OK) {
        initialized_ = true;
    }
    return ret;
}

esp_err_t FloatSwitch::deinit()
{
    initialized_ = false;
    return gpio_hal_.reset_pin(cfg_.gpio);
}

bool FloatSwitch::is_contact_closed()
{
    if (!initialized_)
        return false;
// --- Debouncing Logic ---
// To avoid false readings from electrical noise or contact bounce,
// we take multiple samples over a short period.
const uint8_t samples = DEBOUNCE_SAMPLES;
const uint32_t delay_us = cfg_.debounce_time_us / samples;
uint8_t high_count      = 0;

for (uint8_t i = 0; i < samples; i++) {
    if (gpio_hal_.get_level(cfg_.gpio)) {
        high_count++;
    }
    // A small delay is crucial for the debouncing to be effective.
    timer_hal_.delay_us(delay_us);
}

// A stable reading is determined by a majority vote of the samples.
// `raw_stable` will be true if the GPIO level was mostly HIGH, false if mostly
// LOW.
bool raw_stable = (high_count > (samples / 2));


    // Finally, translate the stable electrical level into the contact state.
    // This depends on whether the closed contact pulls the signal LOW or HIGH.
    if (cfg_.active_level == ActiveLevel::LOW) {
        // If active level is LOW, a closed contact means the raw signal is LOW.
        return !raw_stable;
    }
    else { // ActiveLevel::HIGH
        // If active level is HIGH, a closed contact means the raw signal is HIGH.
        return raw_stable;
    }
}

bool FloatSwitch::is_tank_full()
{
    // First, determine the raw electrical state of the contact.
    bool contact_closed = is_contact_closed();

    // Second, interpret the electrical state to determine the physical state (tank
    // full/empty). This logic depends on the switch type (Normally Open or Normally
    // Closed).
    if (cfg_.normally_open) {
        // For a Normally Open (NO) switch:
        // - Contact is OPEN when the tank is full (float is up).
        // - Contact is CLOSED when the tank is empty (float is down).
        // Therefore, the tank is full if the contact is NOT closed.
        return !contact_closed;
    }
    else {
        // For a Normally Closed (NC) switch:
        // - Contact is CLOSED when the tank is full (float is up).
        // - Contact is OPEN when the tank is empty (float is down).
        // Therefore, the tank is full if the contact IS closed.
        return contact_closed; // NC: Full when closed
    }
}

bool FloatSwitch::should_enable_wakeup()
{
    if (!initialized_)
        return false;

    switch (cfg_.wakeup_on) {
    case WakeupCondition::NEVER:
        // If wake-up is never desired, simply return false.
        return false;

    case WakeupCondition::WHEN_TANK_IS_EMPTY:
        // We want to wake up when the tank becomes empty.
        // To prevent an immediate wake-up loop, we should only arm the wake-up
        // trigger if the tank is currently FULL. This prepares the system to
        // catch the transition from full to empty.
        return is_tank_full();
    case WakeupCondition::WHEN_TANK_IS_FULL:
        // We want to wake up when the tank becomes full.
        // Similarly, we only arm the wake-up trigger if the tank is currently
        // EMPTY. This prepares the system to catch the transition from empty to
        // full.
        return !is_tank_full();
    default:
        return false;
    }
}

} // namespace floatswitch
