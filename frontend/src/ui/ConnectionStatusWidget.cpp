// SIREN Military-Grade Sonar System
// Connection Status Widget Implementation
// Single Responsibility: Display Connection State ONLY

#include "ui/ConnectionStatusWidget.h"
#include "ui/Theme.h"
#include <QHBoxLayout>
#include <QLabel>

namespace siren {
namespace ui {

ConnectionStatusWidget::ConnectionStatusWidget(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_indicator(nullptr)
    , m_statusLabel(nullptr)
    , m_addressLabel(nullptr)
    , m_currentState(ConnectionState::DISCONNECTED)
    , m_serverAddress("localhost:8080")
{
    initializeUI();
    applyMilitaryTheme();
    updateConnectionState(ConnectionState::DISCONNECTED);
}

void ConnectionStatusWidget::updateConnectionState(ConnectionState state)
{
    m_currentState = state;

    if (m_indicator != nullptr && m_statusLabel != nullptr) {
        const QString statusText = getStatusText(state);
        const QString indicatorColor = getIndicatorColor(state);

        // Update status text
        m_statusLabel->setText(statusText);

        // Update indicator color (circular indicator)
        const QString indicatorStyle = QString(
            "QLabel {"
            "    background-color: %1;"
            "    border: 2px solid %1;"
            "    border-radius: %2px;"
            "    min-width: %3px;"
            "    max-width: %3px;"
            "    min-height: %3px;"
            "    max-height: %3px;"
            "}"
        ).arg(indicatorColor)
         .arg(INDICATOR_SIZE / 2)
         .arg(INDICATOR_SIZE);

        m_indicator->setStyleSheet(indicatorStyle);
    }
}

void ConnectionStatusWidget::updateServerAddress(const QString& address)
{
    m_serverAddress = address;

    if (m_addressLabel != nullptr) {
        m_addressLabel->setText(QString("Server: %1").arg(address));
    }
}

void ConnectionStatusWidget::initializeUI()
{
    // Create main horizontal layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN);
    m_layout->setSpacing(WIDGET_SPACING);

    // Create indicator (colored circle)
    m_indicator = new QLabel(this);
    m_indicator->setFixedSize(INDICATOR_SIZE, INDICATOR_SIZE);
    m_indicator->setObjectName("connectionIndicator");

    // Create status text label
    m_statusLabel = new QLabel(DISCONNECTED_TEXT, this);
    m_statusLabel->setObjectName("connectionStatusText");

    // Create server address label
    m_addressLabel = new QLabel(QString("Server: %1").arg(m_serverAddress), this);
    m_addressLabel->setObjectName("connectionAddressText");

    // Add widgets to layout
    m_layout->addWidget(m_indicator);
    m_layout->addWidget(m_statusLabel);
    m_layout->addStretch(); // Push address to the right
    m_layout->addWidget(m_addressLabel);

    setLayout(m_layout);
}

void ConnectionStatusWidget::applyMilitaryTheme()
{
    // Apply military theme styling
    Theme::applyConnectionIndicatorStyle(this);

    // Additional widget-specific styling
    const QString widgetStyle = QString(
        "ConnectionStatusWidget {"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "#connectionStatusText {"
        "    color: #FFFFFF;"
        "    font-family: 'Courier New', monospace;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "#connectionAddressText {"
        "    color: #CCCCCC;"
        "    font-family: 'Courier New', monospace;"
        "    font-size: 12px;"
        "}"
    );

    setStyleSheet(widgetStyle);
}

QString ConnectionStatusWidget::getStatusText(ConnectionState state)
{
    switch (state) {
        case ConnectionState::DISCONNECTED:
            return QString(DISCONNECTED_TEXT);
        case ConnectionState::CONNECTING:
            return QString(CONNECTING_TEXT);
        case ConnectionState::CONNECTED:
            return QString(CONNECTED_TEXT);
    }

    // MISRA C++ Rule 16.1.1: All switch statements shall have a default clause
    return QString(DISCONNECTED_TEXT);
}

QString ConnectionStatusWidget::getIndicatorColor(ConnectionState state)
{
    switch (state) {
        case ConnectionState::DISCONNECTED:
            return QString(DISCONNECTED_COLOR);
        case ConnectionState::CONNECTING:
            return QString(CONNECTING_COLOR);
        case ConnectionState::CONNECTED:
            return QString(CONNECTED_COLOR);
    }

    // MISRA C++ Rule 16.1.1: All switch statements shall have a default clause
    return QString(DISCONNECTED_COLOR);
}

} // namespace ui
} // namespace siren