/**
 * @file config.hpp
 * @brief Central configuration constants for unoRadar military-grade server
 * @author unoRadar Project
 * @date 2025
 *
 * Single Source of Truth (SSOT) for all system configuration parameters.
 * No magic numbers, all values are properly documented and configurable.
 */

#pragma once

#include <chrono>
#include <string>

namespace unoradar::config {

// ============================================================================
// SERIAL COMMUNICATION CONFIGURATION
// ============================================================================

namespace serial {
    /// Default Arduino serial port path (macOS)
    constexpr const char* DEFAULT_PORT_PATH = "/dev/cu.usbmodem*";

    /// Arduino communication baud rate (must match Arduino sketch)
    constexpr int BAUD_RATE = 9600;

    /// Maximum time to wait for serial connection
    constexpr auto CONNECTION_TIMEOUT = std::chrono::seconds(5);

    /// Serial read timeout for Arduino responses
    constexpr auto READ_TIMEOUT = std::chrono::milliseconds(100);

    /// Maximum serial buffer size for incoming data
    constexpr size_t BUFFER_SIZE = 1024;

    /// Expected Arduino data format: "Angle: XXX - Distance: YYY"
    constexpr const char* DATA_FORMAT_REGEX = R"(^Angle:\s*(\d+)\s*-\s*Distance:\s*(\d+)$)";
}

// ============================================================================
// WEBSOCKET SERVER CONFIGURATION
// ============================================================================

namespace websocket {
    /// WebSocket server listening port
    constexpr int DEFAULT_PORT = 8080;

    /// Maximum number of concurrent WebSocket connections
    constexpr size_t MAX_CONNECTIONS = 50;

    /// WebSocket ping interval for keepalive
    constexpr auto PING_INTERVAL = std::chrono::seconds(30);

    /// Connection timeout for inactive clients
    constexpr auto CONNECTION_TIMEOUT = std::chrono::minutes(5);

    /// Maximum message size (bytes)
    constexpr size_t MAX_MESSAGE_SIZE = 4096;
}

// ============================================================================
// RADAR DATA CONFIGURATION
// ============================================================================

namespace radar {
    /// Minimum valid angle in degrees (Arduino servo limit)
    constexpr int MIN_ANGLE = 15;

    /// Maximum valid angle in degrees (Arduino servo limit)
    constexpr int MAX_ANGLE = 165;

    /// Minimum valid distance in centimeters (HC-SR04 limit)
    constexpr int MIN_DISTANCE = 2;

    /// Maximum valid distance in centimeters (HC-SR04 limit)
    constexpr int MAX_DISTANCE = 400;

    /// Angular step size for radar sweep (degrees)
    constexpr int ANGLE_STEP = 2;

    /// Maximum data points to buffer for visualization
    constexpr size_t MAX_BUFFER_SIZE = 1000;

    /// Data expiration time for old measurements
    constexpr auto DATA_EXPIRY_TIME = std::chrono::seconds(10);
}

// ============================================================================
// PERFORMANCE & TIMING CONFIGURATION
// ============================================================================

namespace performance {
    /// Event loop iteration target time (military-grade responsiveness)
    constexpr auto TARGET_LOOP_TIME = std::chrono::microseconds(1000);  // 1ms

    /// Maximum acceptable processing latency
    constexpr auto MAX_PROCESSING_LATENCY = std::chrono::milliseconds(5);

    /// Thread pool size for concurrent operations
    constexpr size_t THREAD_POOL_SIZE = 4;

    /// Memory pool allocation block size
    constexpr size_t MEMORY_POOL_BLOCK_SIZE = 1024;

    /// High-priority thread CPU affinity (if supported)
    constexpr bool ENABLE_CPU_AFFINITY = true;
}

// ============================================================================
// LOGGING CONFIGURATION
// ============================================================================

namespace logging {
    /// Default log level for production
    enum class Level {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    constexpr Level DEFAULT_LEVEL = Level::INFO;
    constexpr Level DEBUG_LEVEL = Level::DEBUG;

    /// Maximum log file size before rotation
    constexpr size_t MAX_LOG_FILE_SIZE = 10 * 1024 * 1024;  // 10MB

    /// Number of log files to keep in rotation
    constexpr size_t LOG_FILE_ROTATION_COUNT = 5;

    /// Log timestamp format
    constexpr const char* TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S.%f";
}

// ============================================================================
// ERROR HANDLING CONFIGURATION
// ============================================================================

namespace error_handling {
    /// Maximum number of retry attempts for recoverable errors
    constexpr int MAX_RETRY_ATTEMPTS = 3;

    /// Base delay between retry attempts
    constexpr auto RETRY_BASE_DELAY = std::chrono::milliseconds(100);

    /// Maximum delay between retry attempts (exponential backoff)
    constexpr auto RETRY_MAX_DELAY = std::chrono::seconds(5);

    /// Enable automatic error recovery
    constexpr bool ENABLE_AUTO_RECOVERY = true;
}

// ============================================================================
// SYSTEM INFORMATION
// ============================================================================

namespace system {
    /// Server version information
    constexpr const char* VERSION = "1.0.0";
    constexpr const char* BUILD_TYPE = "military-grade";
    constexpr const char* AUTHOR = "unoRadar Project";

    /// Server identification for WebSocket handshake
    constexpr const char* SERVER_NAME = "unoRadar-server";
    constexpr const char* USER_AGENT = "unoRadar/1.0.0 (military-grade)";
}

} // namespace unoradar::config