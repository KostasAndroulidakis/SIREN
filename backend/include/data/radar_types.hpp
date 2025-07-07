/**
 * @file radar_types.hpp
 * @brief Core data types for unoRadar military-grade server
 * @author unoRadar Project
 * @date 2025
 *
 * Defines all data structures used throughout the radar system.
 * Following SRP - each type has a single, well-defined responsibility.
 */

#pragma once

#include <chrono>
#include <cstdint>

namespace unoradar {
namespace data {

// ============================================================================
// RADAR MEASUREMENT TYPES
// ============================================================================

/// Single radar measurement point
struct RadarDataPoint {
    /// Servo angle in degrees (15-165 range)
    int16_t angle;

    /// Measured distance in centimeters (2-400 range)
    int16_t distance;

    /// Timestamp when measurement was taken
    std::chrono::steady_clock::time_point timestamp;

    /// Quality indicator (0-100, higher is better)
    uint8_t quality;

    /// Default constructor
    RadarDataPoint()
        : angle(0), distance(0), timestamp(std::chrono::steady_clock::now()), quality(0) {}

    /// Constructor with values
    RadarDataPoint(int16_t a, int16_t d, uint8_t q = 100)
        : angle(a), distance(d), timestamp(std::chrono::steady_clock::now()), quality(q) {}
};

/// Sweep direction for radar operation
enum class SweepDirection : uint8_t {
    FORWARD = 0,   ///< Sweeping from min to max angle
    BACKWARD = 1,  ///< Sweeping from max to min angle
    STATIONARY = 2 ///< Not moving (single point measurement)
};

/// Radar sweep state information
struct SweepState {
    /// Current sweep direction
    SweepDirection direction;

    /// Current angle position
    int16_t current_angle;

    /// Target angle for current movement
    int16_t target_angle;

    /// Number of completed sweeps
    uint32_t sweep_count;

    /// Timestamp of last angle change
    std::chrono::steady_clock::time_point last_movement;

    /// Default constructor
    SweepState()
        : direction(SweepDirection::FORWARD)
        , current_angle(90)
        , target_angle(90)
        , sweep_count(0)
        , last_movement(std::chrono::steady_clock::now()) {}
};

// ============================================================================
// SERIAL COMMUNICATION TYPES
// ============================================================================

/// Serial port connection status
enum class SerialStatus : uint8_t {
    DISCONNECTED = 0,  ///< No connection to Arduino
    CONNECTING = 1,    ///< Attempting to connect
    CONNECTED = 2,     ///< Successfully connected
    ERROR = 3,         ///< Connection error occurred
    TIMEOUT = 4        ///< Connection timeout
};

/// Parsed Arduino message
struct ArduinoMessage {
    /// Servo angle from Arduino
    int16_t angle;

    /// Distance measurement from Arduino
    int16_t distance;

    /// Raw message string received
    std::string raw_data;

    /// Parse success indicator
    bool valid;

    /// Message receive timestamp
    std::chrono::steady_clock::time_point timestamp;

    /// Default constructor
    ArduinoMessage()
        : angle(0), distance(0), valid(false)
        , timestamp(std::chrono::steady_clock::now()) {}
};

// ============================================================================
// NETWORK COMMUNICATION TYPES
// ============================================================================

/// WebSocket client connection state
enum class ConnectionState : uint8_t {
    CONNECTING = 0,    ///< Client is connecting
    CONNECTED = 1,     ///< Client successfully connected
    DISCONNECTING = 2, ///< Client is disconnecting
    DISCONNECTED = 3,  ///< Client disconnected
    ERROR = 4          ///< Connection error
};

/// WebSocket message types
enum class MessageType : uint8_t {
    RADAR_DATA = 0,      ///< Real-time radar measurement
    STATUS_UPDATE = 1,   ///< System status information
    CONTROL_COMMAND = 2, ///< Control command from client
    ERROR_REPORT = 3,    ///< Error notification
    KEEPALIVE = 4        ///< Connection keepalive ping
};

/// WebSocket message structure
struct WebSocketMessage {
    /// Message type identifier
    MessageType type;

    /// JSON payload data
    std::string payload;

    /// Message creation timestamp
    std::chrono::steady_clock::time_point timestamp;

    /// Message size in bytes
    size_t size;

    /// Default constructor
    WebSocketMessage()
        : type(MessageType::RADAR_DATA), size(0)
        , timestamp(std::chrono::steady_clock::now()) {}

    /// Constructor with type and payload
    WebSocketMessage(MessageType t, const std::string& p)
        : type(t), payload(p), size(p.length())
        , timestamp(std::chrono::steady_clock::now()) {}
};

// ============================================================================
// SYSTEM PERFORMANCE TYPES
// ============================================================================

/// Performance metrics for system monitoring
struct PerformanceMetrics {
    /// Messages processed per second
    uint32_t messages_per_second;

    /// Average processing latency in microseconds
    uint32_t avg_latency_us;

    /// Maximum processing latency in microseconds
    uint32_t max_latency_us;

    /// Memory usage in bytes
    size_t memory_usage_bytes;

    /// Active WebSocket connections count
    uint16_t active_connections;

    /// Serial port status
    SerialStatus serial_status;

    /// Timestamp of metrics collection
    std::chrono::steady_clock::time_point timestamp;

    /// Default constructor
    PerformanceMetrics()
        : messages_per_second(0), avg_latency_us(0), max_latency_us(0)
        , memory_usage_bytes(0), active_connections(0)
        , serial_status(SerialStatus::DISCONNECTED)
        , timestamp(std::chrono::steady_clock::now()) {}
};

// ============================================================================
// ERROR HANDLING TYPES
// ============================================================================

/// Error severity levels
enum class ErrorSeverity : uint8_t {
    INFO = 0,     ///< Informational message
    WARNING = 1,  ///< Warning condition
    ERROR = 2,    ///< Error condition
    CRITICAL = 3, ///< Critical system error
    FATAL = 4     ///< Fatal error requiring restart
};

/// System error information
struct SystemError {
    /// Error severity level
    ErrorSeverity severity;

    /// Error code identifier
    uint32_t error_code;

    /// Human-readable error message
    std::string message;

    /// Source component that generated the error
    std::string source;

    /// Error occurrence timestamp
    std::chrono::steady_clock::time_point timestamp;

    /// Default constructor
    SystemError()
        : severity(ErrorSeverity::INFO), error_code(0)
        , timestamp(std::chrono::steady_clock::now()) {}

    /// Constructor with parameters
    SystemError(ErrorSeverity sev, uint32_t code, const std::string& msg, const std::string& src)
        : severity(sev), error_code(code), message(msg), source(src)
        , timestamp(std::chrono::steady_clock::now()) {}
};

// ============================================================================
// SERIAL COMMUNICATION TYPES
// ============================================================================

/// Serial communication statistics
struct SerialStatistics {
    /// Total messages received from Arduino
    uint64_t messages_received;

    /// Total messages sent to Arduino
    uint64_t messages_sent;

    /// Messages received per second (calculated)
    double messages_per_second;

    /// Total parsing errors encountered
    uint32_t parse_errors;

    /// Total connection errors
    uint32_t connection_errors;

    /// Last successful message timestamp
    std::chrono::steady_clock::time_point last_message_time;

    /// Connection uptime in seconds
    uint64_t uptime_seconds;

    /// Average message processing time in microseconds
    uint32_t avg_processing_time_us;

    /// Default constructor
    SerialStatistics()
        : messages_received(0), messages_sent(0), messages_per_second(0.0)
        , parse_errors(0), connection_errors(0)
        , last_message_time(std::chrono::steady_clock::now())
        , uptime_seconds(0), avg_processing_time_us(0) {}
};

} // namespace data
} // namespace unoradar