/**
 * @file json_serializer.cpp
 * @brief Implementation of JSON serialization utility
 * @author SIREN Project
 * @date 2025
 */

#include "utils/json_serializer.hpp"
#include "constants/message.hpp"
#include <sstream>

namespace siren::utils {

namespace constants = siren::constants;

std::string JsonSerializer::serialize(const data::RadarDataPoint& data) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, constants::message::json_types::RADAR_DATA, true) << ","
        << formatField(constants::message::json_fields::TIMESTAMP, data.timestamp_us) << ","
        << formatField(constants::message::json_fields::ANGLE, data.angle) << ","
        << formatField(constants::message::json_fields::DISTANCE, data.distance) << ","
        << formatField(constants::message::json_fields::QUALITY, static_cast<int>(data.quality))
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::PerformanceMetrics& metrics) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, constants::message::json_types::PERFORMANCE_METRICS, true) << ","
        << formatField(constants::message::json_fields::MESSAGES_PER_SECOND, metrics.messages_per_second) << ","
        << formatField(constants::message::json_fields::AVG_LATENCY_US, metrics.avg_latency_us) << ","
        << formatField(constants::message::json_fields::MAX_LATENCY_US, metrics.max_latency_us) << ","
        << formatField(constants::message::json_fields::MEMORY_USAGE_BYTES, metrics.memory_usage_bytes) << ","
        << formatField(constants::message::json_fields::ACTIVE_CONNECTIONS, metrics.active_connections) << ","
        << formatField(constants::message::json_fields::SERIAL_STATUS, static_cast<int>(metrics.serial_status))
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::SystemError& error) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, constants::message::json_types::ERROR_REPORT, true) << ","
        << formatField(constants::message::json_fields::SEVERITY, static_cast<int>(error.severity)) << ","
        << formatField(constants::message::json_fields::ERROR_CODE, error.error_code) << ","
        << formatField(constants::message::json_fields::MESSAGE, error.message, true) << ","
        << formatField(constants::message::json_fields::SOURCE, error.source, true) << ","
        << formatTimestamp(error.timestamp)
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::WebSocketStatistics& stats) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, "websocket_statistics", true) << ","
        << formatField("connections_accepted", stats.connections_accepted) << ","
        << formatField("messages_sent", stats.messages_sent) << ","
        << formatField("connection_errors", stats.connection_errors) << ","
        << formatField(constants::message::json_fields::ACTIVE_CONNECTIONS, stats.active_connections) << ","
        << formatField("uptime_seconds", stats.uptime_seconds)
        << "}";
    return oss.str();
}

std::string JsonSerializer::createStatusUpdate(const std::string& status) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, constants::message::json_types::STATUS_UPDATE, true) << ","
        << formatField("status", status, true) << ","
        << formatField(constants::message::json_fields::TIMESTAMP,
                      std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch()).count())
        << "}";
    return oss.str();
}

std::string JsonSerializer::createKeepalive() {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::message::json_fields::TYPE, constants::message::json_types::KEEPALIVE, true) << ","
        << formatField(constants::message::json_fields::TIMESTAMP,
                      std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch()).count())
        << "}";
    return oss.str();
}

// Template definition moved to header for MISRA C++ compliance
// Explicit instantiations for required types
template<typename T>
std::string JsonSerializer::formatField(const char* key, const T& value, bool is_string) {
    std::ostringstream oss;
    oss << "\"" << key << "\":";

    if (is_string) {
        oss << "\"" << value << "\"";
    } else {
        oss << value;
    }

    return oss.str();
}

// Explicit instantiations for MISRA C++ compliance
template std::string JsonSerializer::formatField<int>(const char* key, const int& value, bool is_string);
template std::string JsonSerializer::formatField<uint32_t>(const char* key, const uint32_t& value, bool is_string);
template std::string JsonSerializer::formatField<uint64_t>(const char* key, const uint64_t& value, bool is_string);
template std::string JsonSerializer::formatField<double>(const char* key, const double& value, bool is_string);
template std::string JsonSerializer::formatField<std::string>(const char* key, const std::string& value, bool is_string);
template std::string JsonSerializer::formatField<const char*>(const char* key, const char* const& value, bool is_string);

// Non-template overloads to match header declarations
std::string JsonSerializer::formatField(const char* key, const int& value, bool is_string) {
    return formatField<int>(key, value, is_string);
}

std::string JsonSerializer::formatField(const char* key, const uint32_t& value, bool is_string) {
    return formatField<uint32_t>(key, value, is_string);
}

std::string JsonSerializer::formatField(const char* key, const uint64_t& value, bool is_string) {
    return formatField<uint64_t>(key, value, is_string);
}

std::string JsonSerializer::formatField(const char* key, const double& value, bool is_string) {
    return formatField<double>(key, value, is_string);
}

std::string JsonSerializer::formatField(const char* key, const std::string& value, bool is_string) {
    return formatField<std::string>(key, value, is_string);
}

std::string JsonSerializer::formatField(const char* key, const char* value, bool is_string) {
    return formatField<const char*>(key, value, is_string);
}

std::string JsonSerializer::formatTimestamp(const std::chrono::steady_clock::time_point& timestamp) {
    auto timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
        timestamp.time_since_epoch()).count();
    return formatField(constants::message::json_fields::TIMESTAMP, timestamp_us);
}

std::string JsonSerializer::formatTimestamp(uint64_t timestamp_us) {
    return formatField(constants::message::json_fields::TIMESTAMP, timestamp_us);
}

} // namespace siren::utils