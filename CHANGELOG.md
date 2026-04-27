# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.2] - 2026-04-26

### Changed
- Renamed interface headers: `i_hal_gpio.hpp` to `i_fs_hal_gpio.hpp` and `i_hal_timer.hpp` to `i_fs_hal_timer.hpp` to avoid naming conflicts.


## [1.0.1] - 2026-04-23

### Changed
- Inlined `GpioHAL` and `TimerHAL` implementations into their headers for better performance.

---

## [1.0.0] - 2026-04-22

### Added
- Initial release of FloatSwitch component for ESP-IDF
- Robust C++ interface for FloatSwitch sensors
- Hardware Abstraction Layer (HAL) for GPIO and Timers
- Comprehensive unit and integration tests

[1.0.1]: https://github.com/aluiziotomazelli/floatswitch/releases/tag/v1.0.1
[1.0.0]: https://github.com/aluiziotomazelli/floatswitch/releases/tag/v1.0.0