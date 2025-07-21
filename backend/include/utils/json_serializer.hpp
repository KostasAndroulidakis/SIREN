/**
 * @file json_serializer.hpp
 * @brief Single responsibility JSON serialization utility
 * @author SIREN Project
 * @date 2025
 *
 * SSOT for all JSON serialization operations.
 * Eliminates duplicate JSON logic throughout the system.
 */

#pragma once

#include <string>
#include "data/sonar_types.hpp"

namespace siren::utils {

/**
 * @brief JSON serializer with single responsibility
 *
 * Handles only JSON serialization operations.
 * Uses constants from SSOT to eliminate hardcoded strings.
 */
class JsonSerializer {
public:
    /**
     * @brief Serialize sonar data point to JSON
     * @param data Sonar data point to serialize
     * @return JSON string representation
     */
    static std::string serialize(const data::SonarDataPoint& data);

    /**
     * @brief Serialize performance metrics to JSON
     * @param metrics Performance metrics to serialize
     * @return JSON string representation
     */
    static std::string serialize(const data::PerformanceMetrics& metrics);

    /**
     * @brief Serialize system error to JSON
     * @param error System error to serialize
     * @return JSON string representation
     */
    static std::string serialize(const data::SystemError& error);

    /**
     * @brief Serialize WebSocket statistics to JSON
     * @param stats WebSocket statistics to serialize
     * @return JSON string representation
     */
    static std::string serialize(const data::WebSocketStatistics& stats);

    /**
     * @brief Create status update message
     * @param status Status message content
     * @return JSON string representation
     */
    static std::string createStatusUpdate(const std::string& status);

    /**
     * @brief Create keepalive message
     * @return JSON string representation
     */
    static std::string createKeepalive();

private:
    /**
     * @brief Helper to format JSON field
     * @param key Field name
     * @param value Field value
     * @param is_string Whether value should be quoted
     * @return Formatted JSON field
     */
    template<typename T>
    static std::string formatField(const char* key, const T& value, bool is_string = false);

    // Explicit instantiation declarations for MISRA C++ compliance
    static std::string formatField(const char* key, const int& value, bool is_string = false);
    static std::string formatField(const char* key, const uint32_t& value, bool is_string = false);
    static std::string formatField(const char* key, const uint64_t& value, bool is_string = false);
    static std::string formatField(const char* key, const double& value, bool is_string = false);
    static std::string formatField(const char* key, const std::string& value, bool is_string = false);
    static std::string formatField(const char* key, const char* value, bool is_string = false);

    /**
     * @brief Helper to create timestamp field
     * @param timestamp Timestamp to format
     * @return Formatted timestamp field
     */
    static std::string formatTimestamp(const std::chrono::steady_clock::time_point& timestamp);

    /**
     * @brief Helper to create timestamp field from microseconds
     * @param timestamp_us Timestamp in microseconds
     * @return Formatted timestamp field
     */
    static std::string formatTimestamp(uint64_t timestamp_us);
};

} // namespace siren::utils