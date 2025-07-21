/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file SonarDataParser.h
 * @brief Parser for sonar data JSON messages
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_DATA_SONAR_DATA_PARSER_H
#define SIREN_DATA_SONAR_DATA_PARSER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <optional>

namespace siren {
namespace Data {

/**
 * @brief Sonar data structure
 * MISRA C++ 2008: 11-0-1 - All members public (POD type)
 */
struct SonarData {
    double angle;        // Degrees (0-180)
    double distance;     // Centimeters (2-450)
    qint64 timestamp;    // Unix timestamp in milliseconds
    double quality;      // Data quality indicator (0-1)

    // Validation method
    bool isValid() const noexcept;
};

/**
 * @brief Parser for sonar data JSON messages
 *
 * Single Responsibility: Parse and validate sonar data only
 * MISRA C++ 2008: 0-1-11 - All methods used
 */
class SonarDataParser : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(SonarDataParser)

public:
    explicit SonarDataParser(QObject* parent = nullptr);
    ~SonarDataParser() override = default;

    // Parsing methods
    std::optional<SonarData> parse(const QString& jsonMessage) const;
    std::optional<SonarData> parse(const QJsonObject& jsonObject) const;

    // Validation methods
    bool validateAngle(double angle) const noexcept;
    bool validateDistance(double distance) const noexcept;

    // Error information
    QString lastError() const;

signals:
    void parseError(const QString& error);
    void dataValidated(const SonarData& data);

private:
    // Private methods
    bool extractAndValidateData(const QJsonObject& obj, SonarData& data) const;

    // Member variables
    mutable QString m_lastError;
};

} // namespace Data
} // namespace siren

#endif // SIREN_DATA_SONAR_DATA_PARSER_H