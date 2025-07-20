/**
 * @file arduino_protocol_parser.cpp
 * @brief Implementation of military-grade Arduino protocol parser
 * @author SIREN Project
 * @date 2025
 *
 * High-performance Arduino message parsing with hardware validation.
 */

#include "serial/arduino_protocol_parser.hpp"
#include "constants/hardware.hpp"
#include "constants/performance.hpp"
#include <iostream>
#include <chrono>

namespace siren::serial {

namespace constants = siren::constants;

ArduinoProtocolParser::ArduinoProtocolParser()
    : pattern_(constants::hardware::arduino::DATA_FORMAT_REGEX)
{
    std::cout << "[ArduinoProtocolParser] Initializing military-grade Arduino protocol parser..." << std::endl;
    std::cout << "[ArduinoProtocolParser] ✅ Parser initialized with regex pattern" << std::endl;
}

std::optional<data::RadarDataPoint> ArduinoProtocolParser::parseRadarData(const std::string& message) {
    auto parsing_start = std::chrono::steady_clock::now();

    try {
        // Expected format: "Angle: X - Distance: Y"
        std::smatch matches;

        if (std::regex_search(message, matches, pattern_)) {
            int angle = std::stoi(matches[1].str());
            int distance = std::stoi(matches[2].str());

            data::RadarDataPoint point(angle, distance);

            // Calculate parsing time
            auto parsing_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - parsing_start).count();

            if (validateHardwareConstraints(point)) {
                updateStatistics(static_cast<uint32_t>(parsing_time), true, true);
                return point;
            } else {
                std::cout << "[ArduinoProtocolParser] ⚠️ Invalid radar data: angle="
                          << angle << "°, distance=" << distance << "cm" << std::endl;
                updateStatistics(static_cast<uint32_t>(parsing_time), true, false);
            }
        } else {
            std::cout << "[ArduinoProtocolParser] ⚠️ Failed to parse message: " << message << std::endl;

            auto parsing_time = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - parsing_start).count();
            updateStatistics(static_cast<uint32_t>(parsing_time), false, false);
        }

    } catch (const std::exception& e) {
        std::cout << "[ArduinoProtocolParser] ❌ Parse exception: " << e.what() << std::endl;

        auto parsing_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - parsing_start).count();
        updateStatistics(static_cast<uint32_t>(parsing_time), false, false);
    }

    return std::nullopt;
}

bool ArduinoProtocolParser::validateHardwareConstraints(const data::RadarDataPoint& data_point) const {
    // Validate angle range against SG90 servo specifications
    if (data_point.angle < constants::hardware::servo::MIN_ANGLE_DEGREES ||
        data_point.angle > constants::hardware::servo::MAX_ANGLE_DEGREES) {
        return false;
    }

    // Validate distance range against HC-SR04 sensor specifications
    if (data_point.distance < constants::hardware::sensor::MIN_DISTANCE_CM ||
        data_point.distance > constants::hardware::sensor::MAX_DISTANCE_CM) {
        return false;
    }

    return true;
}

ArduinoProtocolParser::ParsingStatistics ArduinoProtocolParser::getStatistics() const {
    return statistics_;
}

void ArduinoProtocolParser::resetStatistics() {
    statistics_ = ParsingStatistics{};
    std::cout << "[ArduinoProtocolParser] 📊 Statistics reset" << std::endl;
}

void ArduinoProtocolParser::updateStatistics(uint32_t parsing_time_us, bool parse_successful, bool validation_passed) const {
    statistics_.total_messages_processed++;

    if (parse_successful) {
        statistics_.successful_parses++;

        if (validation_passed) {
            // Only count as successful if both parsing and validation passed
        } else {
            statistics_.validation_failures++;
        }
    } else {
        statistics_.failed_parses++;
    }

    // Update average parsing time using exponential moving average
    if (statistics_.avg_parsing_time_us == 0) {
        statistics_.avg_parsing_time_us = parsing_time_us;
    } else {
        auto alpha = constants::performance::optimization::MOVING_AVERAGE_ALPHA;
        statistics_.avg_parsing_time_us = static_cast<uint32_t>(
            alpha * static_cast<double>(parsing_time_us) +
            (1.0 - alpha) * static_cast<double>(statistics_.avg_parsing_time_us)
        );
    }
}

} // namespace siren::serial