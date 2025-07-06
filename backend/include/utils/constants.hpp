/**
 * @file constants.hpp
 * @brief Global constants and compile-time configurations
 * @author unoRadar Project
 * @date 2025
 *
 * Contains all compile-time constants used across the system.
 * No magic numbers - everything is properly named and documented.
 */

#pragma once

#include <cstdint>

namespace unoradar::constants {

// ============================================================================
// HARDWARE SPECIFICATIONS
// ============================================================================

/// Arduino hardware configuration
namespace arduino {
    /// Arduino serial communication baud rate
    constexpr uint32_t SERIAL_BAUD_RATE = 9600;

    /// Expected data format from Arduino
    constexpr const char* DATA_FORMAT = "Angle: {} - Distance: {}";

    /// Maximum expected response time from Arduino (ms)
    constexpr uint32_t MAX_RESPONSE_TIME_MS = 100;
}

/// Servo motor (SG90) specifications
namespace servo {
    /// Minimum servo angle in degrees
    constexpr int16_t MIN_ANGLE_DEGREES = 15;

    /// Maximum servo angle in degrees
    constexpr int16_t MAX_ANGLE_DEGREES = 165;

    /// Servo step size for military-grade precision
    constexpr int16_t STEP_SIZE_DEGREES = 2;

    /// Servo settling time in milliseconds
    constexpr uint32_t SETTLING_TIME_MS = 20;

    /// Total sweep range in degrees
    constexpr int16_t SWEEP_RANGE_DEGREES = MAX_ANGLE_DEGREES - MIN_ANGLE_DEGREES;

    /// Number of steps in full sweep
    constexpr uint16_t STEPS_PER_SWEEP = SWEEP_RANGE_DEGREES / STEP_SIZE_DEGREES;
}

/// Ultrasonic sensor (HC-SR04) specifications
namespace sensor {
    /// Minimum reliable distance in centimeters
    constexpr int16_t MIN_DISTANCE_CM = 2;

    /// Maximum reliable distance in centimeters
    constexpr int16_t MAX_DISTANCE_CM = 400;

    /// Sensor measurement accuracy in centimeters
    constexpr float ACCURACY_CM = 0.3f;

    /// Sound speed in air at 20°C (cm/μs)
    constexpr float SOUND_SPEED_CM_PER_US = 0.0343f;

    /// Sensor measurement timeout in microseconds
    constexpr uint32_t MEASUREMENT_TIMEOUT_US = 30000;
}

// ============================================================================
// NETWORK PROTOCOL CONSTANTS
// ============================================================================

/// WebSocket server configuration
namespace websocket {
    /// Default server port
    constexpr uint16_t DEFAULT_PORT = 8080;

    /// Maximum concurrent connections
    constexpr uint16_t MAX_CONNECTIONS = 50;

    /// WebSocket protocol version
    constexpr uint8_t PROTOCOL_VERSION = 13;

    /// Maximum message size in bytes
    constexpr uint32_t MAX_MESSAGE_SIZE_BYTES = 4096;

    /// Connection keepalive interval in seconds
    constexpr uint32_t KEEPALIVE_INTERVAL_SEC = 30;

    /// Connection timeout in seconds
    constexpr uint32_t CONNECTION_TIMEOUT_SEC = 300;
}

/// JSON message format constants
namespace json {
    /// Maximum JSON nesting depth
    constexpr uint8_t MAX_NESTING_DEPTH = 10;

    /// Maximum string field length
    constexpr uint16_t MAX_STRING_LENGTH = 1024;

    /// Expected JSON fields for radar data
    constexpr const char* FIELD_ANGLE = "angle";
    constexpr const char* FIELD_DISTANCE = "distance";
    constexpr const char* FIELD_TIMESTAMP = "timestamp";
    constexpr const char* FIELD_TYPE = "type";
    constexpr const char* FIELD_PAYLOAD = "payload";
}

// ============================================================================
// PERFORMANCE AND TIMING CONSTANTS
// ============================================================================

/// Real-time performance requirements
namespace performance {
    /// Target event loop iteration time in microseconds
    constexpr uint32_t TARGET_LOOP_TIME_US = 1000;  // 1ms for military-grade response

    /// Maximum acceptable latency in microseconds
    constexpr uint32_t MAX_LATENCY_US = 5000;  // 5ms maximum

    /// Performance monitoring interval in seconds
    constexpr uint32_t MONITORING_INTERVAL_SEC = 1;

    /// Memory allocation block size in bytes
    constexpr uint32_t MEMORY_BLOCK_SIZE_BYTES = 1024;

    /// Thread pool size for concurrent operations
    constexpr uint8_t THREAD_POOL_SIZE = 4;

    /// High-priority thread stack size in bytes
    constexpr uint32_t HIGH_PRIORITY_STACK_SIZE = 65536;  // 64KB
}

/// Buffer and queue sizes
namespace buffers {
    /// Serial receive buffer size in bytes
    constexpr uint32_t SERIAL_BUFFER_SIZE_BYTES = 1024;

    /// Radar data point buffer size (number of points)
    constexpr uint32_t RADAR_DATA_BUFFER_SIZE = 1000;

    /// WebSocket message queue size
    constexpr uint32_t WEBSOCKET_QUEUE_SIZE = 100;

    /// Error log buffer size
    constexpr uint32_t ERROR_LOG_BUFFER_SIZE = 500;

    /// Performance metrics history size
    constexpr uint32_t METRICS_HISTORY_SIZE = 3600;  // 1 hour at 1Hz
}

// ============================================================================
// ERROR HANDLING AND RECOVERY CONSTANTS
// ============================================================================

/// Error handling configuration
namespace error_handling {
    /// Maximum retry attempts for recoverable errors
    constexpr uint8_t MAX_RETRY_ATTEMPTS = 3;

    /// Base retry delay in milliseconds
    constexpr uint32_t BASE_RETRY_DELAY_MS = 100;

    /// Maximum retry delay in milliseconds (exponential backoff)
    constexpr uint32_t MAX_RETRY_DELAY_MS = 5000;

    /// Error code ranges
    constexpr uint32_t SERIAL_ERROR_BASE = 1000;
    constexpr uint32_t NETWORK_ERROR_BASE = 2000;
    constexpr uint32_t DATA_ERROR_BASE = 3000;
    constexpr uint32_t SYSTEM_ERROR_BASE = 4000;
}

/// Logging configuration
namespace logging {
    /// Maximum log file size in bytes
    constexpr uint32_t MAX_LOG_FILE_SIZE_BYTES = 10 * 1024 * 1024;  // 10MB

    /// Number of log files to rotate
    constexpr uint8_t LOG_FILE_ROTATION_COUNT = 5;

    /// Log flush interval in seconds
    constexpr uint32_t LOG_FLUSH_INTERVAL_SEC = 5;

    /// Maximum log line length
    constexpr uint16_t MAX_LOG_LINE_LENGTH = 2048;
}

// ============================================================================
// MATHEMATICAL CONSTANTS
// ============================================================================

/// Mathematical values for radar calculations
namespace math {
    /// Pi constant for angular calculations
    constexpr double PI = 3.14159265358979323846;

    /// Degrees to radians conversion factor
    constexpr double DEG_TO_RAD = PI / 180.0;

    /// Radians to degrees conversion factor
    constexpr double RAD_TO_DEG = 180.0 / PI;

    /// Radar coordinate system constants
    constexpr double RADAR_CENTER_X = 0.0;
    constexpr double RADAR_CENTER_Y = 0.0;

    /// Distance measurement precision
    constexpr double DISTANCE_PRECISION = 0.1;  // 1mm precision
}

// ============================================================================
// VERSION AND BUILD INFORMATION
// ============================================================================

/// Build and version constants
namespace version {
    /// Major version number
    constexpr uint8_t MAJOR = 1;

    /// Minor version number
    constexpr uint8_t MINOR = 0;

    /// Patch version number
    constexpr uint8_t PATCH = 0;

    /// Build type identifier
    constexpr const char* BUILD_TYPE = "military-grade";

    /// Protocol version for client compatibility
    constexpr uint8_t PROTOCOL_VERSION = 1;

    /// API version for WebSocket communication
    constexpr uint8_t API_VERSION = 1;
}

} // namespace unoradar::constants