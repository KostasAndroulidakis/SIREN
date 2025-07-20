/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file RadarDataParser.h
 * @brief Parser for radar data JSON messages
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_DATA_RADAR_DATA_PARSER_H
#define SIREN_DATA_RADAR_DATA_PARSER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <optional>

namespace siren {
namespace Data {

/**
 * @brief Radar data structure
 * MISRA C++ 2008: 11-0-1 - All members public (POD type)
 */
struct RadarData {
    double angle;        // Degrees (0-180)
    double distance;     // Centimeters (2-450)
    qint64 timestamp;    // Unix timestamp in milliseconds
    double quality;      // Data quality indicator (0-1)
    
    // Validation method
    bool isValid() const noexcept;
};

/**
 * @brief Parser for radar data JSON messages
 * 
 * Single Responsibility: Parse and validate radar data only
 * MISRA C++ 2008: 0-1-11 - All methods used
 */
class RadarDataParser : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(RadarDataParser)

public:
    explicit RadarDataParser(QObject* parent = nullptr);
    ~RadarDataParser() override = default;

    // Parsing methods
    std::optional<RadarData> parse(const QString& jsonMessage) const;
    std::optional<RadarData> parse(const QJsonObject& jsonObject) const;
    
    // Validation methods
    bool validateAngle(double angle) const noexcept;
    bool validateDistance(double distance) const noexcept;
    
    // Error information
    QString lastError() const;

signals:
    void parseError(const QString& error);
    void dataValidated(const RadarData& data);

private:
    // Private methods
    bool extractAndValidateData(const QJsonObject& obj, RadarData& data) const;
    
    // Member variables
    mutable QString m_lastError;
};

} // namespace Data
} // namespace siren

#endif // SIREN_DATA_RADAR_DATA_PARSER_H