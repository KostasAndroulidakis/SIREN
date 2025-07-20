#ifndef SIREN_MILITARY_MAIN_WINDOW_H
#define SIREN_MILITARY_MAIN_WINDOW_H

// SIREN Military-Grade Radar System
// Main Window - Single Responsibility: Application Window Management
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QMainWindow>

namespace siren {
namespace ui {

// Forward declarations
class MainLayout;
class ConnectionStatusWidget;
class SonarDataWidget;

} // namespace ui

namespace Network {
class IWebSocketClient;
} // namespace Network

namespace ui {

/**
 * @brief Military-grade main window - Single Responsibility: Window Management
 * 
 * This window has ONE job: Manage the main application window lifecycle.
 * It does NOT create UI components, handle networking, or process data.
 * 
 * Features:
 * - MainLayout integration for panel arrangement
 * - ConnectionStatusWidget integration
 * - Military theme application
 * - Window state management
 * 
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 */
class MilitaryMainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct military main window
     * @param parent Parent widget (Qt memory management)
     */
    explicit MilitaryMainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~MilitaryMainWindow() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    MilitaryMainWindow(const MilitaryMainWindow&) = delete;
    MilitaryMainWindow& operator=(const MilitaryMainWindow&) = delete;
    MilitaryMainWindow(MilitaryMainWindow&&) = delete;
    MilitaryMainWindow& operator=(MilitaryMainWindow&&) = delete;

private:
    /**
     * @brief Initialize the main window UI
     */
    void initializeUI();

    /**
     * @brief Apply military theme styling
     */
    void applyMilitaryTheme();

    /**
     * @brief Create all UI panels and widgets
     */
    void createPanels();

    /**
     * @brief Initialize WebSocket client and connect to backend
     */
    void initializeWebSocketClient();

    // UI Components (managed by Qt parent-child hierarchy)
    MainLayout* m_mainLayout{nullptr};
    ConnectionStatusWidget* m_connectionStatus{nullptr};
    SonarDataWidget* m_sonarDataWidget{nullptr};
    
    // Network components
    Network::IWebSocketClient* m_webSocketClient{nullptr};
};

} // namespace ui
} // namespace siren

#endif // SIREN_MILITARY_MAIN_WINDOW_H