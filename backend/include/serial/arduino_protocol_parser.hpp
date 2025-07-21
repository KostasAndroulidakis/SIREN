/**
 * @file arduino_protocol_parser.hpp
 * @brief Military-grade Arduino protocol parser for sonar data
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Handles parsing and validation of Arduino serial protocol messages.
 * Implements SRP: Single responsibility for message parsing and validation.
 */

#pragma once

#include <string>
#include <optional>
#include <regex>

#include "data/sonar_types.hpp"

namespace siren::serial {

/**
 * @brief Arduino protocol parser for sonar data messages
 *
 * Military-grade protocol parser with:
 * - Regex-based message parsing
 * - Hardware constraint validation
 * - Error handling and reporting
 * - Performance optimization
 */
class ArduinoProtocolParser {
public:
    /**
     * @brief Constructor - initializes parser with compiled regex
     */
    ArduinoProtocolParser();

    /**
     * @brief Destructor - cleanup resources
     */
    ~ArduinoProtocolParser() = default;

    // Non-copyable, non-movable for thread safety
    ArduinoProtocolParser(const ArduinoProtocolParser&) = delete;
    ArduinoProtocolParser& operator=(const ArduinoProtocolParser&) = delete;
    ArduinoProtocolParser(ArduinoProtocolParser&&) = delete;
    ArduinoProtocolParser& operator=(ArduinoProtocolParser&&) = delete;

    /**
     * @brief Parse sonar data from Arduino message
     * @param message Complete message from Arduino
     * @return Parsed sonar data point, or nullopt if parsing failed
     */
    std::optional<data::SonarDataPoint> parseSonarData(const std::string& message);

    /**
     * @brief Validate sonar data point against hardware constraints
     * @param data_point Data point to validate
     * @return true if data is valid according to hardware specifications
     */
    bool validateHardwareConstraints(const data::SonarDataPoint& data_point) const;

    /**
     * @brief Get parsing statistics
     * @return Parsing performance metrics
     */
    struct ParsingStatistics {
        uint64_t total_messages_processed;
        uint64_t successful_parses;
        uint64_t failed_parses;
        uint64_t validation_failures;
        uint32_t avg_parsing_time_us;

        ParsingStatistics()
            : total_messages_processed(0), successful_parses(0)
            , failed_parses(0), validation_failures(0)
            , avg_parsing_time_us(0) {}
    };

    /**
     * @brief Get current parsing statistics
     * @return Current parsing performance metrics
     */
    ParsingStatistics getStatistics() const;

    /**
     * @brief Reset parsing statistics
     */
    void resetStatistics();

private:
    /// Compiled regex pattern for efficient parsing
    std::regex pattern_;

    /// Parsing statistics (mutable for const getStatistics())
    mutable ParsingStatistics statistics_;

    /**
     * @brief Update parsing statistics
     * @param parsing_time_us Time taken for parsing in microseconds
     * @param parse_successful Whether parsing was successful
     * @param validation_passed Whether validation passed
     */
    void updateStatistics(uint32_t parsing_time_us, bool parse_successful, bool validation_passed) const;
};

} // namespace siren::serial