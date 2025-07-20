// SIREN Military-Grade Radar System
// Main Window Implementation
// Single Responsibility: Application Window Management ONLY

#include "ui/MilitaryMainWindow.h"
#include "ui/MainLayout.h"
#include "ui/PanelFactory.h"
#include "ui/MilitaryTheme.h"
#include "ui/ConnectionStatusWidget.h"
#include "constants/LayoutConstants.h"
#include <QWidget>

namespace siren {
namespace ui {

MilitaryMainWindow::MilitaryMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_mainLayout(nullptr)
    , m_connectionStatus(nullptr)
{
    initializeUI();
    applyMilitaryTheme();
    createPanels();
}

void MilitaryMainWindow::initializeUI()
{
    // Set window properties
    setWindowTitle("SIREN - Military-Grade Radar System");
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

void MilitaryMainWindow::applyMilitaryTheme()
{
    // Apply military theme to application (SRP: only styling)
    MilitaryTheme::applyToApplication(this);
}

void MilitaryMainWindow::createPanels()
{
    // Create connection status widget (SRP: only displays connection state)
    m_connectionStatus = new ConnectionStatusWidget(this);
    
    // Create placeholder panels for other components
    QFrame* statusPanel = PanelFactory::createPanel(PanelFactory::PanelType::STATUS, this);
    QFrame* controlPanel = PanelFactory::createPlaceholder("CONTROL PANEL", this);
    QFrame* radarPanel = PanelFactory::createPlaceholder("RADAR DISPLAY", this);
    QFrame* dataPanel = PanelFactory::createPlaceholder("DATA PANEL", this);
    QFrame* performancePanel = PanelFactory::createPlaceholder("PERFORMANCE METRICS", this);
    
    // Apply military theme styling to panels
    MilitaryTheme::applyStatusPanelStyle(statusPanel);
    MilitaryTheme::applyControlPanelStyle(controlPanel);
    MilitaryTheme::applyRadarPanelStyle(radarPanel);
    MilitaryTheme::applyDataPanelStyle(dataPanel);
    MilitaryTheme::applyPerformancePanelStyle(performancePanel);
    
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
    
    // Set widgets in main layout (SRP: MainLayout only arranges)
    m_mainLayout->setStatusWidget(statusPanel);
    m_mainLayout->setControlWidget(controlPanel);
    m_mainLayout->setCenterWidget(radarPanel);
    m_mainLayout->setDataWidget(dataPanel);
    m_mainLayout->setPerformanceWidget(performancePanel);
    
    // Initialize connection status (start as disconnected)
    m_connectionStatus->updateConnectionState(ConnectionStatusWidget::ConnectionState::DISCONNECTED);
    m_connectionStatus->updateServerAddress("localhost:8080");
}

} // namespace ui
} // namespace siren