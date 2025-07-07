/**
 * @file json_serializer.cpp
 * @brief Implementation of JSON serialization utility
 * @author unoRadar Project
 * @date 2025
 */

#include "utils/json_serializer.hpp"
#include "utils/constants.hpp"
#include <sstream>

namespace unoradar::utils {

namespace constants = unoradar::constants;

std::string JsonSerializer::serialize(const data::RadarDataPoint& data) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, constants::json_types::RADAR_DATA, true) << ","
        << formatField(constants::json_fields::TIMESTAMP, data.timestamp_us) << ","
        << formatField(constants::json_fields::ANGLE, data.angle) << ","
        << formatField(constants::json_fields::DISTANCE, data.distance) << ","
        << formatField(constants::json_fields::QUALITY, static_cast<int>(data.quality))
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::PerformanceMetrics& metrics) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, constants::json_types::PERFORMANCE_METRICS, true) << ","
        << formatField(constants::json_fields::MESSAGES_PER_SECOND, metrics.messages_per_second) << ","
        << formatField(constants::json_fields::AVG_LATENCY_US, metrics.avg_latency_us) << ","
        << formatField(constants::json_fields::MAX_LATENCY_US, metrics.max_latency_us) << ","
        << formatField(constants::json_fields::MEMORY_USAGE_BYTES, metrics.memory_usage_bytes) << ","
        << formatField(constants::json_fields::ACTIVE_CONNECTIONS, metrics.active_connections) << ","
        << formatField(constants::json_fields::SERIAL_STATUS, static_cast<int>(metrics.serial_status))
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::SystemError& error) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, constants::json_types::ERROR_REPORT, true) << ","
        << formatField(constants::json_fields::SEVERITY, static_cast<int>(error.severity)) << ","
        << formatField(constants::json_fields::ERROR_CODE, error.error_code) << ","
        << formatField(constants::json_fields::MESSAGE, error.message, true) << ","
        << formatField(constants::json_fields::SOURCE, error.source, true) << ","
        << formatTimestamp(error.timestamp)
        << "}";
    return oss.str();
}

std::string JsonSerializer::serialize(const data::WebSocketStatistics& stats) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, "websocket_statistics", true) << ","
        << formatField("connections_accepted", stats.connections_accepted) << ","
        << formatField("messages_sent", stats.messages_sent) << ","
        << formatField("connection_errors", stats.connection_errors) << ","
        << formatField(constants::json_fields::ACTIVE_CONNECTIONS, stats.active_connections) << ","
        << formatField("uptime_seconds", stats.uptime_seconds)
        << "}";
    return oss.str();
}

std::string JsonSerializer::createStatusUpdate(const std::string& status) {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, constants::json_types::STATUS_UPDATE, true) << ","
        << formatField("status", status, true) << ","
        << formatField(constants::json_fields::TIMESTAMP,
                      std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch()).count())
        << "}";
    return oss.str();
}

std::string JsonSerializer::createKeepalive() {
    std::ostringstream oss;
    oss << "{"
        << formatField(constants::json_fields::TYPE, constants::json_types::KEEPALIVE, true) << ","
        << formatField(constants::json_fields::TIMESTAMP,
                      std::chrono::duration_cast<std::chrono::microseconds>(
                          std::chrono::steady_clock::now().time_since_epoch()).count())
        << "}";
    return oss.str();
}

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

std::string JsonSerializer::formatTimestamp(const std::chrono::steady_clock::time_point& timestamp) {
    auto timestamp_us = std::chrono::duration_cast<std::chrono::microseconds>(
        timestamp.time_since_epoch()).count();
    return formatField(constants::json_fields::TIMESTAMP, timestamp_us);
}

std::string JsonSerializer::formatTimestamp(uint64_t timestamp_us) {
    return formatField(constants::json_fields::TIMESTAMP, timestamp_us);
}

} // namespace unoradar::utils