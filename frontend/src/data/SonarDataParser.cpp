// SIREN Military-Grade Sonar System
// Sonar Data Parser Implementation
// Single Responsibility: Parse JSON Sonar Messages ONLY

#include "data/SonarDataParser.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDateTime>

namespace siren {
namespace data {

bool SonarDataPoint::isWithinHardwareLimits() const
{
    // HC-SR04 ultrasonic sensor: 2cm to 400cm
    // SG90 servo motor: 0° to 180°
    return (angle <= 180) && 
           (distance >= 2 && distance <= 400) && 
           valid;
}

QString SonarDataPoint::toString() const
{
    if (!valid) {
        return QString("Invalid sonar data");
    }
    
    return QString("Angle: %1°, Distance: %2cm, Time: %3ms")
           .arg(angle)
           .arg(distance)
           .arg(timestamp);
}

SonarDataParser::ParseResult SonarDataParser::parseMessage(const QJsonObject& jsonMessage, 
                                                           SonarDataPoint& dataPoint)
{
    // Reset data point
    dataPoint = SonarDataPoint{};
    
    // Check if message has required type field
    if (!jsonMessage.contains(MESSAGE_TYPE_FIELD)) {
        return ParseResult::MISSING_FIELDS;
    }
    
    const QString messageType = jsonMessage[MESSAGE_TYPE_FIELD].toString();
    
    // Support both "sonar_data" and legacy "radar_data" for compatibility
    if (messageType != SONAR_DATA_TYPE && messageType != RADAR_DATA_TYPE) {
        return ParseResult::UNKNOWN_MESSAGE;
    }
    
    // Support both nested and flat JSON formats
    QJsonObject dataObj;
    
    if (jsonMessage.contains(DATA_FIELD)) {
        // Nested format: {"type":"radar_data","data":{"angle":90,"distance":150}}
        dataObj = jsonMessage[DATA_FIELD].toObject();
    } else {
        // Flat format: {"type":"radar_data","angle":90,"distance":150}
        dataObj = jsonMessage;
    }
    
    // Extract sonar data from data object
    if (!extractSonarData(dataObj, dataPoint)) {
        return ParseResult::MISSING_FIELDS;
    }
    
    // Validate hardware constraints
    if (!validateHardwareConstraints(dataPoint)) {
        // Determine specific validation failure
        if (dataPoint.angle > MAX_SERVO_ANGLE) {
            return ParseResult::INVALID_ANGLE;
        }
        if (dataPoint.distance < MIN_SENSOR_DISTANCE || dataPoint.distance > MAX_SENSOR_DISTANCE) {
            return ParseResult::INVALID_DISTANCE;
        }
        return ParseResult::INVALID_ANGLE; // Default to angle error
    }
    
    dataPoint.valid = true;
    return ParseResult::SUCCESS;
}

SonarDataParser::ParseResult SonarDataParser::parseJsonText(const QString& jsonText, 
                                                            SonarDataPoint& dataPoint)
{
    // Reset data point
    dataPoint = SonarDataPoint{};
    
    // Parse JSON text
    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonText.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        return ParseResult::INVALID_JSON;
    }
    
    if (!jsonDoc.isObject()) {
        return ParseResult::INVALID_JSON;
    }
    
    const QJsonObject jsonObj = jsonDoc.object();
    return parseMessage(jsonObj, dataPoint);
}

bool SonarDataParser::validateHardwareConstraints(const SonarDataPoint& dataPoint)
{
    // Validate servo angle (SG90: 0° to 180°)
    if (dataPoint.angle > MAX_SERVO_ANGLE) {
        return false;
    }
    
    // Validate ultrasonic distance (HC-SR04: 2cm to 400cm)
    if (dataPoint.distance < MIN_SENSOR_DISTANCE || dataPoint.distance > MAX_SENSOR_DISTANCE) {
        return false;
    }
    
    return true;
}

QString SonarDataParser::getErrorDescription(ParseResult result)
{
    switch (result) {
        case ParseResult::SUCCESS:
            return QString("Success");
        case ParseResult::INVALID_JSON:
            return QString("Invalid JSON format");
        case ParseResult::MISSING_FIELDS:
            return QString("Missing required fields");
        case ParseResult::INVALID_ANGLE:
            return QString("Angle out of range (0-180°)");
        case ParseResult::INVALID_DISTANCE:
            return QString("Distance out of range (2-400cm)");
        case ParseResult::UNKNOWN_MESSAGE:
            return QString("Unknown message type");
    }
    
    // MISRA C++ Rule 16.1.1: All switch statements shall have a default clause
    return QString("Unknown error");
}

bool SonarDataParser::extractSonarData(const QJsonObject& jsonObj, 
                                       SonarDataPoint& dataPoint)
{
    // Extract angle field
    if (!jsonObj.contains(ANGLE_FIELD)) {
        return false;
    }
    
    const QJsonValue angleValue = jsonObj[ANGLE_FIELD];
    if (!angleValue.isDouble()) {
        return false;
    }
    
    dataPoint.angle = static_cast<std::uint16_t>(angleValue.toInt());
    
    // Extract distance field
    if (!jsonObj.contains(DISTANCE_FIELD)) {
        return false;
    }
    
    const QJsonValue distanceValue = jsonObj[DISTANCE_FIELD];
    if (!distanceValue.isDouble()) {
        return false;
    }
    
    dataPoint.distance = static_cast<std::uint16_t>(distanceValue.toInt());
    
    // Extract timestamp (optional)
    if (jsonObj.contains(TIMESTAMP_FIELD)) {
        const QJsonValue timestampValue = jsonObj[TIMESTAMP_FIELD];
        if (timestampValue.isDouble()) {
            dataPoint.timestamp = static_cast<std::uint64_t>(timestampValue.toDouble());
        } else {
            // Use current time if timestamp is invalid
            dataPoint.timestamp = static_cast<std::uint64_t>(QDateTime::currentMSecsSinceEpoch());
        }
    } else {
        // Use current time if timestamp is missing
        dataPoint.timestamp = static_cast<std::uint64_t>(QDateTime::currentMSecsSinceEpoch());
    }
    
    return true;
}

} // namespace data
} // namespace siren