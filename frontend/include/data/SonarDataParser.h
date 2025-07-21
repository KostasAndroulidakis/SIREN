#ifndef SIREN_SONAR_DATA_PARSER_H
#define SIREN_SONAR_DATA_PARSER_H

// SIREN Military-Grade Sonar System
// Sonar Data Parser - Single Responsibility: Parse JSON Sonar Messages
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QJsonObject>
#include <QString>
#include <cstdint>

namespace siren {
namespace data {

/**
 * @brief Sonar data point from ultrasonic sensor
 *
 * Contains angle (servo position) and distance (ultrasonic measurement)
 * from the Arduino HC-SR04 sensor and SG90 servo system.
 */
struct SonarDataPoint {
    std::uint16_t angle{0};        // Servo angle in degrees (0-180)
    std::uint16_t distance{0};     // Distance in centimeters (2-400)
    std::uint64_t timestamp{0};    // Timestamp in milliseconds
    bool valid{false};             // Data validity flag

    /**
     * @brief Check if sonar data is within valid hardware ranges
     * @return True if data is valid for HC-SR04 and SG90 specs
     */
    [[nodiscard]] bool isWithinHardwareLimits() const;

    /**
     * @brief Get human-readable string representation
     * @return Formatted string (e.g., "Angle: 90°, Distance: 142cm")
     */
    [[nodiscard]] QString toString() const;
};

/**
 * @brief Sonar data parser - Single Responsibility: Parse JSON Sonar Data
 *
 * This class has ONE job: Parse JSON messages from the backend WebSocket.
 * It does NOT handle networking, UI updates, or data storage.
 *
 * Features:
 * - Parse JSON sonar data messages
 * - Validate hardware constraints (HC-SR04, SG90)
 * - Convert to structured SonarDataPoint
 * - Error handling for malformed messages
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 * - Rule 8.18.2: No assignment in conditions
 */
class SonarDataParser final
{
public:
    /**
     * @brief Parse result for sonar data parsing
     */
    enum class ParseResult : std::uint8_t {
        SUCCESS = 0,           // Successfully parsed
        INVALID_JSON = 1,      // Malformed JSON
        MISSING_FIELDS = 2,    // Required fields missing
        INVALID_ANGLE = 3,     // Angle out of range
        INVALID_DISTANCE = 4,  // Distance out of range
        UNKNOWN_MESSAGE = 5    // Unknown message type
    };

    /**
     * @brief Parse JSON message from WebSocket
     * @param jsonMessage JSON object from backend
     * @param dataPoint Output sonar data point
     * @return Parse result status
     */
    [[nodiscard]] static ParseResult parseMessage(const QJsonObject& jsonMessage,
                                                  SonarDataPoint& dataPoint);

    /**
     * @brief Parse JSON text string
     * @param jsonText Raw JSON string from WebSocket
     * @param dataPoint Output sonar data point
     * @return Parse result status
     */
    [[nodiscard]] static ParseResult parseJsonText(const QString& jsonText,
                                                   SonarDataPoint& dataPoint);

    /**
     * @brief Validate sonar data against hardware constraints
     * @param dataPoint Sonar data to validate
     * @return True if data is valid for our hardware
     */
    [[nodiscard]] static bool validateHardwareConstraints(const SonarDataPoint& dataPoint);

    /**
     * @brief Get error description for parse result
     * @param result Parse result code
     * @return Human-readable error description
     */
    [[nodiscard]] static QString getErrorDescription(ParseResult result);

private:
    // Static class - no instantiation
    SonarDataParser() = delete;
    ~SonarDataParser() = delete;
    SonarDataParser(const SonarDataParser&) = delete;
    SonarDataParser& operator=(const SonarDataParser&) = delete;

    /**
     * @brief Extract sonar data from JSON object
     * @param jsonObj JSON object containing sonar data
     * @param dataPoint Output data point
     * @return True if extraction successful
     */
    [[nodiscard]] static bool extractSonarData(const QJsonObject& jsonObj,
                                               SonarDataPoint& dataPoint);

    // Hardware constraints (SSOT for sensor specifications)
    static constexpr std::uint16_t MIN_SERVO_ANGLE = 0;        // SG90 minimum angle
    static constexpr std::uint16_t MAX_SERVO_ANGLE = 180;      // SG90 maximum angle
    static constexpr std::uint16_t MIN_SENSOR_DISTANCE = 2;    // HC-SR04 minimum (cm)
    static constexpr std::uint16_t MAX_SENSOR_DISTANCE = 400;  // HC-SR04 maximum (cm)

    // JSON field names (SSOT for protocol)
    static constexpr const char* MESSAGE_TYPE_FIELD = "type";
    static constexpr const char* DATA_FIELD = "data";
    static constexpr const char* ANGLE_FIELD = "angle";
    static constexpr const char* DISTANCE_FIELD = "distance";
    static constexpr const char* TIMESTAMP_FIELD = "timestamp";
    static constexpr const char* SONAR_DATA_TYPE = "sonar_data";
    static constexpr const char* SONAR_DATA_TYPE = "sonar_data";  // Legacy support
};

} // namespace data
} // namespace siren

#endif // SIREN_SONAR_DATA_PARSER_H