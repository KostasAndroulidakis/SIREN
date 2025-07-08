/**
 * @file hardware.hpp
 * @brief Hardware specifications and constraints for military-grade radar system
 * @author unoRadar Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Physical hardware specifications derived from component datasheets.
 * Contains Arduino UNO R3, SG90 servo, and HC-SR04 sensor specifications.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - hardware specifications only
 */

#pragma once

#include <cstdint>
#include <chrono>

namespace unoradar { namespace constants { namespace hardware {

/// Arduino UNO R3 specifications from ATmega328P datasheet
namespace arduino {
    /// Serial communication baud rate (standard Arduino configuration)
    constexpr uint32_t SERIAL_BAUD_RATE = 9600;

    /// Maximum expected response time from Arduino firmware
    constexpr auto MAX_RESPONSE_TIME = std::chrono::milliseconds(100);

    /// Data format regex pattern for Arduino protocol parsing
    constexpr const char* DATA_FORMAT_REGEX = R"(Angle:\s*(\d+)\s*-\s*Distance:\s*(\d+))";
}

/// SG90 Servo Motor specifications from datasheet
namespace servo {
    /// Minimum servo angle in degrees (physical hardware limit)
    constexpr int16_t MIN_ANGLE_DEGREES = 15;

    /// Maximum servo angle in degrees (physical hardware limit)
    constexpr int16_t MAX_ANGLE_DEGREES = 165;

    /// Angular step size for optimal precision vs speed trade-off
    constexpr int16_t STEP_SIZE_DEGREES = 2;

    /// Servo settling time after movement command (from datasheet)
    constexpr auto SETTLING_TIME = std::chrono::milliseconds(20);

    /// Total sweep range calculation (derived constant)
    constexpr int16_t SWEEP_RANGE_DEGREES = MAX_ANGLE_DEGREES - MIN_ANGLE_DEGREES;

    /// Number of discrete positions in full sweep (derived constant)
    constexpr uint16_t STEPS_PER_SWEEP = SWEEP_RANGE_DEGREES / STEP_SIZE_DEGREES;
}

/// HC-SR04 Ultrasonic Sensor specifications from datasheet
namespace sensor {
    /// Minimum reliable measurement distance (datasheet specification)
    constexpr int16_t MIN_DISTANCE_CM = 2;

    /// Maximum reliable measurement distance (datasheet specification)
    constexpr int16_t MAX_DISTANCE_CM = 400;

    /// Measurement accuracy specification (datasheet)
    constexpr float ACCURACY_CM = 0.3f;

    /// Sound speed in air at 20°C for distance calculation
    constexpr float SOUND_SPEED_CM_PER_US = 0.0343f;

    /// Sensor measurement timeout (datasheet maximum)
    constexpr auto MEASUREMENT_TIMEOUT = std::chrono::microseconds(30000);
}

/// Cross-platform serial port detection
namespace platform {
    /// Platform detection flags (MISRA C++ compliant)
#ifdef _WIN32
    constexpr bool IS_WINDOWS = true;
    constexpr bool IS_MACOS = false;
    constexpr bool IS_LINUX = false;
#elif __APPLE__
    constexpr bool IS_WINDOWS = false;
    constexpr bool IS_MACOS = true;
    constexpr bool IS_LINUX = false;
#elif __linux__
    constexpr bool IS_WINDOWS = false;
    constexpr bool IS_MACOS = false;
    constexpr bool IS_LINUX = true;
#else
    constexpr bool IS_WINDOWS = false;
    constexpr bool IS_MACOS = false;
    constexpr bool IS_LINUX = false;
#endif

    /// Cross-platform serial device paths (SSOT)
#ifdef _WIN32
    constexpr const char* SERIAL_DEVICE_PATH = "COM";
#else
    constexpr const char* SERIAL_DEVICE_PATH = "/dev";
#endif
}

} } } // namespace unoradar::constants::hardware