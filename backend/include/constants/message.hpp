/**
 * @file message.hpp
 * @brief Message formats, JSON fields, and communication protocols for military-grade sonar
 * @author KostasAndroulidakis
 * @date 2025
 * @classification UNCLASSIFIED
 *
 * JSON message structure, field definitions, and protocol versioning.
 * Defines all message types and field names for consistent communication.
 *
 * MISRA C++ Compliance: All constants are constexpr and strongly typed
 * SRP: Single responsibility - message formats and protocol definitions only
 */

#pragma once

#include <cstdint>
#include <chrono>

namespace siren { namespace constants { namespace message {

/// JSON field names - Single Source of Truth for all JSON operations
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

    /// Error handling and reporting fields
    constexpr const char* SEVERITY = "severity";
    constexpr const char* ERROR_CODE = "error_code";
    constexpr const char* MESSAGE = "message";
    constexpr const char* SOURCE = "source";
}

/// JSON message types - Single Source of Truth for message type identification
namespace json_types {
    constexpr const char* SONAR_DATA = "sonar_data";
    constexpr const char* PERFORMANCE_METRICS = "performance_metrics";
    constexpr const char* STATUS_UPDATE = "status_update";
    constexpr const char* ERROR_REPORT = "error_report";
    constexpr const char* KEEPALIVE = "keepalive";
}

/// Version and build information
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
    constexpr const char* AUTHOR = "SIREN Defense Systems";
    constexpr const char* SERVER_NAME = "SIREN Server";
    constexpr const char* USER_AGENT = "SIREN/1.0.0 (military-grade)";
}

/// WebSocket session status messages - Military-appropriate logging
namespace websocket_status {
    constexpr const char* SESSION_STARTING = "Starting session for client";
    constexpr const char* HANDSHAKE_COMPLETE = "WebSocket handshake complete for";
    constexpr const char* CLIENT_DISCONNECTED = "Client disconnected";
    constexpr const char* RECEIVED_MESSAGE = "Received from";
    constexpr const char* CLOSING_CONNECTION = "Closing connection to";
    constexpr const char* ERROR_CLOSING = "Error closing connection";
    constexpr const char* SESSION_PREFIX = "[WebSocketSession]";
    constexpr const char* SERVER_PREFIX = "[WebSocketServer]";
    constexpr const char* INITIALIZING_SERVER = "Initializing military-grade WebSocket server on port";
    constexpr const char* INIT_FAILED_CONNECTION_MANAGER = "Failed to initialize connection manager";
    constexpr const char* INIT_FAILED_BROADCASTER = "Failed to initialize message broadcaster";
    constexpr const char* SERVER_INITIALIZED = "WebSocket server initialized on port";
    constexpr const char* INIT_FAILED_EXCEPTION = "WebSocket server initialization failed";
    constexpr const char* SERVER_ALREADY_RUNNING = "Server already running";
    constexpr const char* START_FAILED_CONNECTION_MANAGER = "Failed to start connection manager";
    constexpr const char* START_FAILED_BROADCASTER = "Failed to start message broadcaster";
    constexpr const char* SERVER_STARTED = "WebSocket server started - listening on port";
    constexpr const char* START_FAILED_EXCEPTION = "WebSocket server start failed";
    constexpr const char* STOPPING_SERVER = "Stopping WebSocket server";
    constexpr const char* SERVER_STOPPED = "WebSocket server stopped";
    constexpr const char* NEW_CLIENT_CONNECTED = "New client connected";
    constexpr const char* TOTAL_CLIENTS = "(total:";
    constexpr const char* UNKNOWN_CLIENT = "unknown";
    constexpr const char* HANDSHAKE_FAILED_ERROR = "WebSocket handshake failed";
    constexpr const char* WRITE_ERROR = "Write error";
}

/// Test and demonstration values
namespace test {
    /// Test controller run duration
    constexpr auto TEST_RUN_DURATION = std::chrono::seconds(2);
}

} } } // namespace siren::constants::message