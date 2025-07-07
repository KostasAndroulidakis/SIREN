/**
 * @file constants.hpp
 * @brief Single Source of Truth (SSOT) for all unoRadar system constants
 * @author unoRadar Project
 * @date 2025
 *
 * Central repository for ALL compile-time constants and configuration values.
 * Eliminates magic numbers and ensures consistent values across the system.
 *
 * SRP: Single responsibility - constant definition and organization
 * SSOT: Single source of truth - all constants defined here ONLY
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <chrono>
#ifdef __cpp_lib_math_constants
#include <numbers>
#endif

namespace unoradar { namespace constants {

// ============================================================================
// HARDWARE SPECIFICATIONS (Physical constraints from hardware datasheets)
// ============================================================================

/// Arduino UNO R3 hardware specifications
namespace arduino {
    /// Serial communication baud rate (Arduino standard)
    constexpr uint32_t SERIAL_BAUD_RATE = 9600;

    /// Maximum expected response time from Arduino
    constexpr auto MAX_RESPONSE_TIME = std::chrono::milliseconds(100);

    /// Expected data format pattern from Arduino
    constexpr const char* DATA_FORMAT_REGEX = R"(Angle:\s*(\d+)\s*-\s*Distance:\s*(\d+))";
}

/// SG90 Servo Motor specifications (from datasheet)
namespace servo {
    /// Minimum servo angle in degrees (hardware limit)
    constexpr int16_t MIN_ANGLE_DEGREES = 15;

    /// Maximum servo angle in degrees (hardware limit)
    constexpr int16_t MAX_ANGLE_DEGREES = 165;

    /// Angular step size for military-grade precision
    constexpr int16_t STEP_SIZE_DEGREES = 2;

    /// Servo settling time after movement command
    constexpr auto SETTLING_TIME = std::chrono::milliseconds(20);

    /// Total sweep range calculation
    constexpr int16_t SWEEP_RANGE_DEGREES = MAX_ANGLE_DEGREES - MIN_ANGLE_DEGREES;

    /// Number of discrete positions in full sweep
    constexpr uint16_t STEPS_PER_SWEEP = SWEEP_RANGE_DEGREES / STEP_SIZE_DEGREES;
}

/// HC-SR04 Ultrasonic Sensor specifications (from datasheet)
namespace sensor {
    /// Minimum reliable measurement distance
    constexpr int16_t MIN_DISTANCE_CM = 2;

    /// Maximum reliable measurement distance
    constexpr int16_t MAX_DISTANCE_CM = 400;

    /// Measurement accuracy specification
    constexpr float ACCURACY_CM = 0.3f;

    /// Sound speed in air at 20°C for distance calculation
    constexpr float SOUND_SPEED_CM_PER_US = 0.0343f;

    /// Sensor measurement timeout
    constexpr auto MEASUREMENT_TIMEOUT = std::chrono::microseconds(30000);
}

// ============================================================================
// SERIAL COMMUNICATION CONSTANTS
// ============================================================================

namespace serial {
    /// Arduino serial baud rate (must match Arduino sketch)
    constexpr uint32_t BAUD_RATE = arduino::SERIAL_BAUD_RATE;

    /// Serial read buffer size for incoming data
    constexpr size_t BUFFER_SIZE = 256;

    /// Maximum length of a single message from Arduino
    constexpr size_t MAX_MESSAGE_LENGTH = 128;

    /// Connection establishment timeout
    constexpr auto CONNECTION_TIMEOUT = std::chrono::seconds(10);

    /// Delay between reconnection attempts
    constexpr auto RECONNECT_DELAY = std::chrono::seconds(5);

    /// Maximum number of reconnection attempts before giving up
    constexpr uint32_t MAX_RECONNECT_ATTEMPTS = 10;

    /// Data timeout - maximum time without receiving data
    constexpr auto DATA_TIMEOUT = std::chrono::seconds(30);

    /// Buffer overflow threshold multiplier
    constexpr size_t BUFFER_OVERFLOW_MULTIPLIER = 2;
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

    /// Estimated memory per WebSocket connection in bytes
    constexpr uint32_t ESTIMATED_CONNECTION_MEMORY_BYTES = 8192;  // 8KB per connection

    /// Base system memory usage estimate in bytes
    constexpr uint32_t BASE_MEMORY_USAGE_BYTES = 1048576;  // 1MB base usage
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

    /// Maximum error message length for security (prevent information leakage)
    constexpr size_t MAX_ERROR_MESSAGE_LENGTH = 512;

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

    /// Full version string
    constexpr const char* VERSION_STRING = "1.0.0";

    /// Build information
    constexpr const char* AUTHOR = "unoRadar Project";
    constexpr const char* SERVER_NAME = "unoRadar-server";
    constexpr const char* USER_AGENT = "unoRadar/1.0.0 (military-grade)";
}

// ============================================================================
// MATHEMATICAL CONSTANTS (Use std::numbers for PI, etc.)
// ============================================================================

namespace math {
    // Use C++20 std::numbers::pi or fallback to high-precision value
    #ifdef __cpp_lib_math_constants
        constexpr double PI = std::numbers::pi;
    #else
        constexpr double PI = 3.14159265358979323846;
    #endif

    /// Degrees to radians conversion factor
    constexpr double DEG_TO_RAD = PI / 180.0;

    /// Radians to degrees conversion factor
    constexpr double RAD_TO_DEG = 180.0 / PI;

    /// Radar coordinate system origin
    constexpr double RADAR_CENTER_X = 0.0;
    constexpr double RADAR_CENTER_Y = 0.0;

    /// Distance measurement precision (1mm)
    constexpr double DISTANCE_PRECISION = 0.1;
}

// ============================================================================
// MAGIC NUMBER ELIMINATION CONSTANTS
// ============================================================================

namespace magic_numbers {
    /// Moving average calculation factor (replaces hardcoded /2)
    constexpr double MOVING_AVERAGE_ALPHA = 0.1;  // Weight for new values in exponential moving average

    /// CPU spin prevention divisor (replaces hardcoded /10)
    constexpr uint32_t SPIN_PREVENTION_DIVISOR = 10;

    /// Health check timeout threshold (replaces hardcoded 5 seconds)
    constexpr uint32_t HEALTH_CHECK_TIMEOUT_SEC = 5;

    /// Boost version display divisors (for clean main.cpp)
    constexpr uint32_t BOOST_VERSION_MAJOR_DIVISOR = 100000;
    constexpr uint32_t BOOST_VERSION_MINOR_DIVISOR = 100;
    constexpr uint32_t BOOST_VERSION_MINOR_MODULO = 1000;
    constexpr uint32_t BOOST_VERSION_PATCH_MODULO = 100;
}

// ============================================================================
// JSON SERIALIZATION CONSTANTS (SSOT for all JSON operations)
// ============================================================================

/// JSON field names - eliminates hardcoded strings
namespace json_fields {
    constexpr const char* TYPE = "type";
    constexpr const char* TIMESTAMP = "timestamp";
    constexpr const char* ANGLE = "angle";
    constexpr const char* DISTANCE = "distance";
    constexpr const char* QUALITY = "quality";
    constexpr const char* MESSAGES_PER_SECOND = "messages_per_second";
    constexpr const char* AVG_LATENCY_US = "avg_latency_us";
    constexpr const char* MAX_LATENCY_US = "max_latency_us";
    constexpr const char* MEMORY_USAGE_BYTES = "memory_usage_bytes";
    constexpr const char* ACTIVE_CONNECTIONS = "active_connections";
    constexpr const char* SERIAL_STATUS = "serial_status";
}

/// JSON message types - SSOT for message type identification
namespace json_types {
    constexpr const char* RADAR_DATA = "radar_data";
    constexpr const char* PERFORMANCE_METRICS = "performance_metrics";
    constexpr const char* STATUS_UPDATE = "status_update";
    constexpr const char* ERROR_REPORT = "error_report";
    constexpr const char* KEEPALIVE = "keepalive";
}

// ============================================================================
// TEST AND DEMONSTRATION VALUES
// ============================================================================

namespace test_values {
    /// Test radar data point values for demonstrations
    constexpr int16_t TEST_ANGLE_DEGREES = 90;
    constexpr int16_t TEST_DISTANCE_CM = 150;

    /// Test controller run duration
    constexpr auto TEST_RUN_DURATION = std::chrono::seconds(2);
}

} } // namespace unoradar::constants