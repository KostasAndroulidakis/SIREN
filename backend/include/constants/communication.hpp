/**
 * @file communication.hpp
 * @brief Communication protocols and network configuration for military-grade radar
 * @author unoRadar Defense Systems
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * Serial communication and WebSocket protocol specifications.
 * Defines all communication timeouts, buffer sizes, and protocol parameters.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - communication protocols only
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <chrono>

namespace unoradar { namespace constants { namespace communication {

/// Serial communication configuration
namespace serial {
    /// Arduino serial baud rate (must match firmware configuration)
    constexpr uint32_t BAUD_RATE = 9600;

    /// Serial read buffer size for incoming data
    constexpr size_t BUFFER_SIZE = 256;

    /// Maximum length of a single message from Arduino
    constexpr size_t MAX_MESSAGE_LENGTH = 128;

    /// Connection establishment timeout
    constexpr auto CONNECTION_TIMEOUT = std::chrono::seconds(10);

    /// Delay between reconnection attempts
    constexpr auto RECONNECT_DELAY = std::chrono::seconds(5);

    /// Maximum number of reconnection attempts before failure
    constexpr uint32_t MAX_RECONNECT_ATTEMPTS = 10;

    /// Data timeout - maximum time without receiving data
    constexpr auto DATA_TIMEOUT = std::chrono::seconds(30);

    /// Buffer overflow threshold multiplier
    constexpr size_t BUFFER_OVERFLOW_MULTIPLIER = 2;

    /// Command terminator character for Arduino communication
    constexpr const char* COMMAND_TERMINATOR = "\n";
}

/// WebSocket server configuration
namespace websocket {
    /// Default server port for client connections
    constexpr uint16_t DEFAULT_PORT = 8080;

    /// Maximum concurrent connections (military-grade limit)
    constexpr uint16_t MAX_CONNECTIONS = 50;

    /// WebSocket protocol version (RFC 6455)
    constexpr uint8_t PROTOCOL_VERSION = 13;

    /// Maximum message size in bytes (security constraint)
    constexpr uint32_t MAX_MESSAGE_SIZE_BYTES = 4096;

    /// Connection keepalive interval in seconds
    constexpr uint32_t KEEPALIVE_INTERVAL_SEC = 30;

    /// Connection timeout in seconds
    constexpr uint32_t CONNECTION_TIMEOUT_SEC = 300;
}

/// JSON message format configuration
namespace json {
    /// Maximum JSON nesting depth (security constraint)
    constexpr uint8_t MAX_NESTING_DEPTH = 10;

    /// Maximum string field length (security constraint)
    constexpr uint16_t MAX_STRING_LENGTH = 1024;

    /// Expected JSON fields for radar data
    constexpr const char* FIELD_ANGLE = "angle";
    constexpr const char* FIELD_DISTANCE = "distance";
    constexpr const char* FIELD_TIMESTAMP = "timestamp";
    constexpr const char* FIELD_TYPE = "type";
    constexpr const char* FIELD_PAYLOAD = "payload";
}

} } } // namespace unoradar::constants::communication