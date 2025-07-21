#ifndef SIREN_MAIN_WINDOW_H
#define SIREN_MAIN_WINDOW_H

// SIREN Main Window
// Single Responsibility: Application Window Management
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QMainWindow>

namespace siren {
namespace ui {

// Forward declarations
class MainLayout;
class ConnectionStatusWidget;
class SonarDataWidget;
class SonarVisualizationWidget;

} // namespace ui

namespace Network {
class IWebSocketClient;
} // namespace Network

namespace ui {

/**
 * @brief Main window - Single Responsibility: Window Management
 * 
 * This window has ONE job: Manage the main application window lifecycle.
 * It does NOT create UI components, handle networking, or process data.
 * 
 * Features:
 * - MainLayout integration for panel arrangement
 * - ConnectionStatusWidget integration
 * - Theme application
 * - Window state management
 * 
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 */
class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct main window
     * @param parent Parent widget (Qt memory management)
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~MainWindow() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    MainWindow& operator=(MainWindow&&) = delete;

private:
    /**
     * @brief Initialize the main window UI
     */
    void initializeUI();

    /**
     * @brief Apply theme styling
     */
    void applyTheme();

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
    SonarVisualizationWidget* m_sonarVisualizationWidget{nullptr};
    
    // Network components
    Network::IWebSocketClient* m_webSocketClient{nullptr};
};

} // namespace ui
} // namespace siren

#endif // SIREN_MAIN_WINDOW_H