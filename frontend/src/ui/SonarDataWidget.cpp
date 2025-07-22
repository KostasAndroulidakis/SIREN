// SIREN Sonar System
// Sonar Data Widget Implementation
// Single Responsibility: Display Sonar Readings ONLY

#include "ui/SonarDataWidget.h"
#include "ui/Theme.h"
#include <QDateTime>
#include <QFont>

namespace siren {
namespace ui {

SonarDataWidget::SonarDataWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_angleValue(nullptr)
    , m_distanceValue(nullptr)
    , m_timestampValue(nullptr)
    , m_qualityIndicator(nullptr)
    , m_statusIndicator(nullptr)
    , m_angleRow(nullptr)
    , m_distanceRow(nullptr)
    , m_timestampRow(nullptr)
    , m_qualityRow(nullptr)
    , m_statusRow(nullptr)
    , m_currentData{}
    , m_dataReceiving(false)
{
    initializeUI();
    applyMilitaryTheme();
    clearData();
}

void SonarDataWidget::updateSonarData(const data::SonarDataPoint& sonarData)
{
    m_currentData = sonarData;

    // Update angle display
    if (sonarData.valid) {
        const QString angleText = QString::number(sonarData.angle) + DEGREE_SYMBOL;
        m_angleValue->setText(angleText);
        m_angleValue->setStyleSheet(QString("color: %1;").arg(getAngleValidationColor(sonarData.angle)));
    } else {
        m_angleValue->setText(NO_DATA_TEXT);
        m_angleValue->setStyleSheet(QString("color: %1;").arg(INVALID_COLOR));
    }

    // Update distance display
    if (sonarData.valid) {
        const QString distanceText = QString::number(sonarData.distance) + CM_UNIT;
        m_distanceValue->setText(distanceText);
        m_distanceValue->setStyleSheet(QString("color: %1;").arg(getDistanceValidationColor(sonarData.distance)));
    } else {
        m_distanceValue->setText(NO_DATA_TEXT);
        m_distanceValue->setStyleSheet(QString("color: %1;").arg(INVALID_COLOR));
    }

    // Update timestamp
    m_timestampValue->setText(formatTimestamp(sonarData.timestamp));

    // Update data quality
    updateDataQuality(sonarData.valid, sonarData.isWithinHardwareLimits());

    // Update reception status
    setDataReceptionStatus(true);
}

void SonarDataWidget::clearData()
{
    // Clear all value displays
    m_angleValue->setText(NO_DATA_TEXT);
    m_distanceValue->setText(NO_DATA_TEXT);
    m_timestampValue->setText(NO_DATA_TEXT);

    // Reset colors to inactive
    m_angleValue->setStyleSheet(QString("color: %1;").arg(INACTIVE_COLOR));
    m_distanceValue->setStyleSheet(QString("color: %1;").arg(INACTIVE_COLOR));
    m_timestampValue->setStyleSheet(QString("color: %1;").arg(INACTIVE_COLOR));

    // Clear quality indicator
    updateDataQuality(false, false);

    // Update status
    setDataReceptionStatus(false);

    // Reset current data
    m_currentData = data::SonarDataPoint{};
}

void SonarDataWidget::setDataReceptionStatus(bool receiving)
{
    m_dataReceiving = receiving;

    if (receiving) {
        m_statusIndicator->setText(RECEIVING_TEXT);
        m_statusIndicator->setStyleSheet(QString("color: %1; font-weight: bold;").arg(VALID_COLOR));
    } else {
        m_statusIndicator->setText(NO_SIGNAL_TEXT);
        m_statusIndicator->setStyleSheet(QString("color: %1; font-weight: bold;").arg(INVALID_COLOR));
    }
}

void SonarDataWidget::initializeUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN);
    m_mainLayout->setSpacing(ROW_SPACING);

    // Create value widgets only - labels are created in createDataRow
    m_angleValue = new QLabel(NO_DATA_TEXT, this);
    m_distanceValue = new QLabel(NO_DATA_TEXT, this);
    m_timestampValue = new QLabel(NO_DATA_TEXT, this);
    m_qualityIndicator = new QLabel(INVALID_TEXT, this);
    m_statusIndicator = new QLabel(NO_SIGNAL_TEXT, this);

    // Label properties are set in createDataRow function

    // Set value properties
    m_angleValue->setMinimumWidth(VALUE_MIN_WIDTH);
    m_distanceValue->setMinimumWidth(VALUE_MIN_WIDTH);
    m_timestampValue->setMinimumWidth(VALUE_MIN_WIDTH);
    m_qualityIndicator->setMinimumWidth(VALUE_MIN_WIDTH);
    m_statusIndicator->setMinimumWidth(VALUE_MIN_WIDTH);

    // Create data rows
    m_angleRow = createDataRow(ANGLE_LABEL_TEXT, m_angleValue);
    m_distanceRow = createDataRow(DISTANCE_LABEL_TEXT, m_distanceValue);
    m_timestampRow = createDataRow(TIMESTAMP_LABEL_TEXT, m_timestampValue);
    m_qualityRow = createDataRow(QUALITY_LABEL_TEXT, m_qualityIndicator);
    m_statusRow = createDataRow(STATUS_LABEL_TEXT, m_statusIndicator);

    // Add rows to main layout
    m_mainLayout->addWidget(m_angleRow);
    m_mainLayout->addWidget(m_distanceRow);
    m_mainLayout->addWidget(m_timestampRow);
    m_mainLayout->addWidget(m_qualityRow);
    m_mainLayout->addWidget(m_statusRow);
    m_mainLayout->addStretch(); // Push content to top
}

void SonarDataWidget::applyMilitaryTheme()
{
    // Apply military theme styling
    const QString widgetStyle = QString(
        "SonarDataWidget {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "QLabel {"
        "    color: #FFFFFF;"
        "    font-family: 'Courier New', monospace;"
        "    font-size: 13px;"
        "    background-color: transparent;"
        "    padding: 2px;"
        "    margin: 1px;"
        "}"
        "QFrame {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
    );

    setStyleSheet(widgetStyle);
}

QFrame* SonarDataWidget::createDataRow(const QString& label, QWidget* valueWidget)
{
    QFrame* row = new QFrame(this);
    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(5, 2, 5, 2);
    layout->setSpacing(20);  // Optimal spacing for readability

    // Create label
    QLabel* labelWidget = new QLabel(label, row);
    labelWidget->setMinimumWidth(LABEL_MIN_WIDTH);
    labelWidget->setMaximumWidth(LABEL_MIN_WIDTH);  // Fixed width to prevent expansion
    labelWidget->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // Set value widget properties
    valueWidget->setMinimumWidth(VALUE_MIN_WIDTH);
    valueWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Set alignment if it's a QLabel
    QLabel* labelValueWidget = qobject_cast<QLabel*>(valueWidget);
    if (labelValueWidget != nullptr) {
        labelValueWidget->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        labelValueWidget->setWordWrap(false);  // Prevent text wrapping
    }

    // Add to layout with proper stretch factors
    layout->addWidget(labelWidget, 0);  // Don't stretch label
    layout->addWidget(valueWidget, 1);  // Allow value to expand

    return row;
}

void SonarDataWidget::updateDataQuality(bool isValid, bool isWithinLimits)
{
    if (isValid && isWithinLimits) {
        m_qualityIndicator->setText(VALID_TEXT);
        m_qualityIndicator->setStyleSheet(QString("color: %1; font-weight: bold;").arg(VALID_COLOR));
    } else if (isValid && !isWithinLimits) {
        m_qualityIndicator->setText("Range Error");
        m_qualityIndicator->setStyleSheet(QString("color: %1; font-weight: bold;").arg(WARNING_COLOR));
    } else {
        m_qualityIndicator->setText(INVALID_TEXT);
        m_qualityIndicator->setStyleSheet(QString("color: %1; font-weight: bold;").arg(INVALID_COLOR));
    }
}

QString SonarDataWidget::formatTimestamp(std::uint64_t timestamp)
{
    if (timestamp == 0) {
        return QString(NO_DATA_TEXT);
    }

    // Convert to QDateTime and format
    const QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(timestamp));
    return dateTime.toString("hh:mm:ss.zzz");
}

QString SonarDataWidget::getAngleValidationColor(std::uint16_t angle)
{
    // SG90 servo: 0° to 180° (valid range)
    if (angle <= 180) {
        return QString(VALID_COLOR);  // Green
    } else {
        return QString(INVALID_COLOR);  // Red
    }
}

QString SonarDataWidget::getDistanceValidationColor(std::uint16_t distance)
{
    // HC-SR04 ultrasonic sensor: 2cm to 400cm (valid range)
    if (distance >= 2 && distance <= 400) {
        if (distance <= 10) {
            return QString(WARNING_COLOR);  // Orange for very close objects
        } else {
            return QString(VALID_COLOR);    // Green for normal range
        }
    } else {
        return QString(INVALID_COLOR);      // Red for out of range
    }
}

} // namespace ui
} // namespace siren