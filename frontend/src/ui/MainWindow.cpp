// SIREN Main Window Implementation
// Single Responsibility: Application Window Management ONLY

#include "ui/MainWindow.h"
#include "ui/MainLayout.h"
#include "ui/PanelFactory.h"
#include "ui/Theme.h"
#include "ui/ConnectionStatusWidget.h"
#include "ui/SonarDataWidget.h"
#include "ui/SonarVisualizationWidget.h"
#include "network/WebSocketClient.h"
#include "data/SonarDataParser.h"
#include "constants/LayoutConstants.h"
#include <QWidget>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace siren {
namespace ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_mainLayout(nullptr)
    , m_connectionStatus(nullptr)
    , m_sonarDataWidget(nullptr)
    , m_sonarVisualizationWidget(nullptr)
    , m_webSocketClient(nullptr)
{
    initializeUI();
    applyTheme();
    createPanels();
    initializeWebSocketClient();
}

void MainWindow::initializeUI()
{
    // Set window properties
    setWindowTitle("SIREN - Sonar Detection System");
    resize(constants::layout::WINDOW_WIDTH, constants::layout::WINDOW_HEIGHT);
    setMinimumSize(constants::layout::MIN_WINDOW_WIDTH, constants::layout::MIN_WINDOW_HEIGHT);

    // Create main layout widget that fills the entire window
    m_mainLayout = new MainLayout(this);

    // Create central widget and set the layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainWindowLayout = new QVBoxLayout(centralWidget);
    mainWindowLayout->setContentsMargins(0, 0, 0, 0);
    mainWindowLayout->setSpacing(0);
    mainWindowLayout->addWidget(m_mainLayout);

    setCentralWidget(centralWidget);
}

void MainWindow::applyTheme()
{
    // Apply theme to application (SRP: only styling)
    Theme::applyToApplication(this);
}

void MainWindow::createPanels()
{
    // Create connection status widget (SRP: only displays connection state)
    m_connectionStatus = new ConnectionStatusWidget(this);

    // Create sonar data widget (SRP: only displays sonar readings)
    m_sonarDataWidget = new SonarDataWidget(this);

    // Create sonar visualization widget (SRP: only renders sonar display)
    m_sonarVisualizationWidget = new SonarVisualizationWidget(this);

    // Create placeholder panels for other components
    QFrame* statusPanel = PanelFactory::createPanel(PanelFactory::PanelType::STATUS, this);
    QFrame* controlPanel = PanelFactory::createPlaceholder("CONTROL PANEL", this);
    QFrame* sonarPanel = PanelFactory::createPanel(PanelFactory::PanelType::SONAR, this);
    QFrame* dataPanel = PanelFactory::createPanel(PanelFactory::PanelType::DATA, this);
    QFrame* performancePanel = PanelFactory::createPlaceholder("PERFORMANCE METRICS", this);

    // Apply theme styling to panels
    Theme::applyStatusPanelStyle(statusPanel);
    Theme::applyControlPanelStyle(controlPanel);
    Theme::applySonarPanelStyle(sonarPanel);
    Theme::applyDataPanelStyle(dataPanel);
    Theme::applyPerformancePanelStyle(performancePanel);

    // Add connection status to status panel
    QHBoxLayout* statusLayout = new QHBoxLayout(statusPanel);
    statusLayout->setContentsMargins(
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN
    );
    statusLayout->addWidget(m_connectionStatus);
    statusLayout->addStretch(); // Push connection status to the left

    // Add sonar data widget to data panel
    QVBoxLayout* dataLayout = new QVBoxLayout(dataPanel);
    dataLayout->setContentsMargins(
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN,
        constants::layout::PANEL_MARGIN
    );
    dataLayout->addWidget(m_sonarDataWidget);
    dataLayout->addStretch(); // Push sonar data to the top

    // Add sonar visualization to sonar panel
    QVBoxLayout* sonarLayout = new QVBoxLayout(sonarPanel);
    sonarLayout->setContentsMargins(0, 0, 0, 0);  // No margins for full display
    sonarLayout->addWidget(m_sonarVisualizationWidget);

    // Set widgets in main layout (SRP: MainLayout only arranges)
    m_mainLayout->setStatusWidget(statusPanel);
    m_mainLayout->setControlWidget(controlPanel);
    m_mainLayout->setCenterWidget(sonarPanel);
    m_mainLayout->setDataWidget(dataPanel);
    m_mainLayout->setPerformanceWidget(performancePanel);

    // Initialize connection status (start as disconnected)
    m_connectionStatus->updateConnectionState(ConnectionStatusWidget::ConnectionState::DISCONNECTED);
    m_connectionStatus->updateServerAddress("localhost:8080");
}

void MainWindow::initializeWebSocketClient()
{
    // Create WebSocket client (SRP: only handles backend communication)
    m_webSocketClient = new Network::WebSocketClient(this);

    // Connect WebSocket state changes to connection status widget
    connect(m_webSocketClient, &Network::IWebSocketClient::stateChanged,
            this, [this](Network::IWebSocketClient::State state) {
                ConnectionStatusWidget::ConnectionState widgetState;

                // Map WebSocket state to widget state
                switch (state) {
                    case Network::IWebSocketClient::State::Disconnected:
                        widgetState = ConnectionStatusWidget::ConnectionState::DISCONNECTED;
                        break;
                    case Network::IWebSocketClient::State::Connecting:
                        widgetState = ConnectionStatusWidget::ConnectionState::CONNECTING;
                        break;
                    case Network::IWebSocketClient::State::Connected:
                        widgetState = ConnectionStatusWidget::ConnectionState::CONNECTED;
                        break;
                    case Network::IWebSocketClient::State::Closing:
                        widgetState = ConnectionStatusWidget::ConnectionState::DISCONNECTED;
                        break;
                }

                m_connectionStatus->updateConnectionState(widgetState);
            });

    // Connect to sonar data messages
    connect(m_webSocketClient, &Network::IWebSocketClient::textMessageReceived,
            this, [this](const QString& message) {
                // Parse incoming sonar data
                data::SonarDataPoint sonarData;
                const auto parseResult = data::SonarDataParser::parseJsonText(message, sonarData);

                if (parseResult == data::SonarDataParser::ParseResult::SUCCESS) {
                    // Successfully parsed sonar data
                    qDebug() << "Sonar data received:" << sonarData.toString();

                    // Update sonar data widget (SRP: only displays data)
                    m_sonarDataWidget->updateSonarData(sonarData);

                    // Update sonar visualization widget (SRP: only renders display)
                    m_sonarVisualizationWidget->updateSonarData(sonarData);
                } else {
                    // Log parsing errors for debugging
                    const QString errorDesc = data::SonarDataParser::getErrorDescription(parseResult);
                    qDebug() << "Failed to parse sonar data:" << errorDesc << "Message:" << message;
                }
            });

    // Connect to backend server automatically
    const QUrl serverUrl("ws://localhost:8080");
    m_webSocketClient->connectToServer(serverUrl);
}

} // namespace ui
} // namespace siren