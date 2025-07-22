// SIREN Sonar System
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
    , m_indicator(nullptr)
    , m_statusLabel(nullptr)
    , m_addressLabel(nullptr)
    , m_currentState(ConnectionState::DISCONNECTED)
    , m_serverAddress("localhost:8080")
{
    initializeUI();
    applyTheme();
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
        m_addressLabel->setText(address); // Just show address without "Server:" prefix
    }
}

void ConnectionStatusWidget::initializeUI()
{
    // Create main vertical layout to match data widget style
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN, WIDGET_MARGIN);
    mainLayout->setSpacing(ROW_SPACING); // Match SonarDataWidget row spacing

    // Create server address row (top) - no frame styling like data widget
    QFrame* serverRow = new QFrame(this);
    QHBoxLayout* serverLayout = new QHBoxLayout(serverRow);
    serverLayout->setContentsMargins(5, 2, 5, 2); // Match SonarDataWidget row margins
    serverLayout->setSpacing(20); // Match SonarDataWidget spacing

    QLabel* serverLabel = new QLabel("Server:", this);
    serverLabel->setMinimumWidth(LABEL_MIN_WIDTH); // Match SonarDataWidget exactly
    serverLabel->setMaximumWidth(LABEL_MIN_WIDTH);
    serverLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_addressLabel = new QLabel(m_serverAddress, this); // Just show address without "Server:" prefix
    m_addressLabel->setObjectName("connectionAddressText");
    m_addressLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    serverLayout->addWidget(serverLabel, 0);
    serverLayout->addWidget(m_addressLabel, 1);

    // Create connection status row (bottom) - no frame styling like data widget
    QFrame* statusRow = new QFrame(this);
    QHBoxLayout* statusLayout = new QHBoxLayout(statusRow);
    statusLayout->setContentsMargins(5, 2, 5, 2);
    statusLayout->setSpacing(20); // Match SonarDataWidget spacing exactly

    QLabel* connectionLabel = new QLabel("Status:", this);
    connectionLabel->setMinimumWidth(LABEL_MIN_WIDTH);
    connectionLabel->setMaximumWidth(LABEL_MIN_WIDTH);
    connectionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create indicator and status text in a horizontal container
    QWidget* statusContainer = new QWidget(this);
    QHBoxLayout* statusContainerLayout = new QHBoxLayout(statusContainer);
    statusContainerLayout->setContentsMargins(0, 0, 0, 0);
    statusContainerLayout->setSpacing(8); // Keep small spacing for indicator-text pair

    m_indicator = new QLabel(this);
    m_indicator->setFixedSize(INDICATOR_SIZE, INDICATOR_SIZE);
    m_indicator->setObjectName("connectionIndicator");

    m_statusLabel = new QLabel(DISCONNECTED_TEXT, this);
    m_statusLabel->setObjectName("connectionStatusText");
    m_statusLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    statusContainerLayout->addWidget(m_indicator);
    statusContainerLayout->addWidget(m_statusLabel);
    statusContainerLayout->addStretch();

    statusLayout->addWidget(connectionLabel, 0);
    statusLayout->addWidget(statusContainer, 1);

    // Add rows to main layout
    mainLayout->addWidget(serverRow);
    mainLayout->addWidget(statusRow);

    setLayout(mainLayout);
}

void ConnectionStatusWidget::applyTheme()
{
    // Apply military theme styling to match SonarDataWidget appearance
    const QString widgetStyle = QString(
        "ConnectionStatusWidget {"
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
        "#connectionStatusText {"
        "    color: #FFFFFF;"
        "    font-family: 'Courier New', monospace;"
        "    font-weight: bold;"
        "    font-size: 13px;"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "#connectionAddressText {"
        "    color: #CCCCCC;"
        "    font-family: 'Courier New', monospace;"
        "    font-size: 13px;"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "#connectionIndicator {"
        "    background-color: transparent;"
        "    border: none;"
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