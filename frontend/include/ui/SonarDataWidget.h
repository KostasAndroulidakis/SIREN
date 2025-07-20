#ifndef SIREN_SONAR_DATA_WIDGET_H
#define SIREN_SONAR_DATA_WIDGET_H

// SIREN Military-Grade Sonar System
// Sonar Data Widget - Single Responsibility: Display Sonar Readings
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "data/SonarDataParser.h"

namespace siren {
namespace ui {

/**
 * @brief Sonar data display widget - Single Responsibility: Show Sonar Readings
 * 
 * This widget has ONE job: Display current sonar data in a structured format.
 * It does NOT parse data, handle networking, or manage data storage.
 * 
 * Features:
 * - Current angle and distance display
 * - Data quality indicators
 * - Timestamp information
 * - Military-grade styling
 * - Hardware range validation display
 * 
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 * - Rule 8.18.2: No assignment in conditions
 */
class SonarDataWidget final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Construct sonar data widget
     * @param parent Parent widget (Qt memory management)
     */
    explicit SonarDataWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~SonarDataWidget() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    SonarDataWidget(const SonarDataWidget&) = delete;
    SonarDataWidget& operator=(const SonarDataWidget&) = delete;
    SonarDataWidget(SonarDataWidget&&) = delete;
    SonarDataWidget& operator=(SonarDataWidget&&) = delete;

public slots:
    /**
     * @brief Update displayed sonar data
     * @param sonarData New sonar data point to display
     */
    void updateSonarData(const data::SonarDataPoint& sonarData);

    /**
     * @brief Clear all displayed data (on disconnect)
     */
    void clearData();

    /**
     * @brief Update data reception status
     * @param receiving True if actively receiving data
     */
    void setDataReceptionStatus(bool receiving);

signals:
    /**
     * @brief Emitted when user requests data clear
     */
    void clearDataRequested();

private:
    /**
     * @brief Initialize UI components
     */
    void initializeUI();

    /**
     * @brief Apply military theme styling
     */
    void applyMilitaryTheme();

    /**
     * @brief Create data display row
     * @param label Label text
     * @param valueWidget Widget to display value
     * @return QFrame containing the row
     */
    [[nodiscard]] QFrame* createDataRow(const QString& label, QWidget* valueWidget);

    /**
     * @brief Update data quality indicator
     * @param isValid True if data is valid
     * @param isWithinLimits True if within hardware limits
     */
    void updateDataQuality(bool isValid, bool isWithinLimits);

    /**
     * @brief Format timestamp for display
     * @param timestamp Timestamp in milliseconds
     * @return Formatted time string
     */
    [[nodiscard]] static QString formatTimestamp(std::uint64_t timestamp);

    /**
     * @brief Get angle validation color
     * @param angle Servo angle in degrees
     * @return CSS color string
     */
    [[nodiscard]] static QString getAngleValidationColor(std::uint16_t angle);

    /**
     * @brief Get distance validation color
     * @param distance Distance in centimeters
     * @return CSS color string
     */
    [[nodiscard]] static QString getDistanceValidationColor(std::uint16_t distance);

    // UI Components (managed by Qt parent-child hierarchy)
    QVBoxLayout* m_mainLayout{nullptr};
    
    // Data display labels
    QLabel* m_angleLabel{nullptr};
    QLabel* m_angleValue{nullptr};
    QLabel* m_distanceLabel{nullptr};
    QLabel* m_distanceValue{nullptr};
    QLabel* m_timestampLabel{nullptr};
    QLabel* m_timestampValue{nullptr};
    QLabel* m_qualityLabel{nullptr};
    QLabel* m_qualityIndicator{nullptr};
    QLabel* m_statusLabel{nullptr};
    QLabel* m_statusIndicator{nullptr};

    // Data rows
    QFrame* m_angleRow{nullptr};
    QFrame* m_distanceRow{nullptr};
    QFrame* m_timestampRow{nullptr};
    QFrame* m_qualityRow{nullptr};
    QFrame* m_statusRow{nullptr};

    // Current data state
    data::SonarDataPoint m_currentData{};
    bool m_dataReceiving{false};

    // Display constants (SSOT for widget appearance)
    static constexpr int WIDGET_MARGIN = 10;
    static constexpr int ROW_SPACING = 8;
    static constexpr int LABEL_MIN_WIDTH = 80;
    static constexpr int VALUE_MIN_WIDTH = 100;
    static constexpr int INDICATOR_SIZE = 12;
    
    // Text constants (SSOT for display strings)
    static constexpr const char* ANGLE_LABEL_TEXT = "Angle:";
    static constexpr const char* DISTANCE_LABEL_TEXT = "Distance:";
    static constexpr const char* TIMESTAMP_LABEL_TEXT = "Time:";
    static constexpr const char* QUALITY_LABEL_TEXT = "Quality:";
    static constexpr const char* STATUS_LABEL_TEXT = "Status:";
    
    static constexpr const char* NO_DATA_TEXT = "---";
    static constexpr const char* DEGREE_SYMBOL = "°";
    static constexpr const char* CM_UNIT = "cm";
    static constexpr const char* VALID_TEXT = "Valid";
    static constexpr const char* INVALID_TEXT = "Invalid";
    static constexpr const char* RECEIVING_TEXT = "Receiving";
    static constexpr const char* NO_SIGNAL_TEXT = "No Signal";
    
    // Color constants (SSOT for validation colors)
    static constexpr const char* VALID_COLOR = "#00FF00";      // Green
    static constexpr const char* INVALID_COLOR = "#FF0000";    // Red
    static constexpr const char* WARNING_COLOR = "#FF4500";    // Orange
    static constexpr const char* NORMAL_COLOR = "#FFFFFF";     // White
    static constexpr const char* INACTIVE_COLOR = "#666666";   // Gray
};

} // namespace ui
} // namespace siren

#endif // SIREN_SONAR_DATA_WIDGET_H