#ifndef SIREN_CONNECTION_STATUS_WIDGET_H
#define SIREN_CONNECTION_STATUS_WIDGET_H

// SIREN Sonar System
// Connection Status Widget - Single Responsibility: Display Connection State
// Compliant with MISRA C++ 2023, SRP, SSOT

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

namespace siren {
namespace ui {

/**
 * @brief Connection status display widget - Single Responsibility: Show Connection State
 *
 * This widget has ONE job: Display the current connection status to the backend.
 * It does NOT manage the connection, handle networking, or control other widgets.
 *
 * Features:
 * - Visual indicator (colored circle)
 * - Status text ("Connected" / "Disconnected" / "Connecting...")
 * - Military-grade styling
 *
 * MISRA C++ Compliance:
 * - Rule 12.4.1: No dynamic allocation after initialization
 * - Rule 21.2.1: RAII for all resources
 * - Rule 5.0.1: No magic numbers (uses constants)
 * - Rule 8.18.2: No assignment in conditions
 */
class ConnectionStatusWidget final : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Connection states that can be displayed
     */
    enum class ConnectionState : std::uint8_t {
        DISCONNECTED = 0,    // Red indicator, "Disconnected"
        CONNECTING = 1,      // Orange indicator, "Connecting..."
        CONNECTED = 2        // Green indicator, "Connected"
    };

    /**
     * @brief Construct connection status widget
     * @param parent Parent widget (Qt memory management)
     */
    explicit ConnectionStatusWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~ConnectionStatusWidget() override = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for Qt widgets
    ConnectionStatusWidget(const ConnectionStatusWidget&) = delete;
    ConnectionStatusWidget& operator=(const ConnectionStatusWidget&) = delete;
    ConnectionStatusWidget(ConnectionStatusWidget&&) = delete;
    ConnectionStatusWidget& operator=(ConnectionStatusWidget&&) = delete;

public slots:
    /**
     * @brief Update the displayed connection state
     * @param state New connection state to display
     */
    void updateConnectionState(ConnectionState state);

    /**
     * @brief Update with backend server address
     * @param address Server address to display (e.g., "localhost:8080")
     */
    void updateServerAddress(const QString& address);

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
     * @brief Get status text for given state
     * @param state Connection state
     * @return Human-readable status text
     */
    [[nodiscard]] static QString getStatusText(ConnectionState state);

    /**
     * @brief Get indicator color for given state
     * @param state Connection state
     * @return CSS color string
     */
    [[nodiscard]] static QString getIndicatorColor(ConnectionState state);

    // UI Components (managed by Qt parent-child hierarchy)
    QHBoxLayout* m_layout{nullptr};
    QLabel* m_indicator{nullptr};      // Colored circle indicator
    QLabel* m_statusLabel{nullptr};    // "Connected" / "Disconnected" text
    QLabel* m_addressLabel{nullptr};   // Server address display

    // Current state
    ConnectionState m_currentState{ConnectionState::DISCONNECTED};
    QString m_serverAddress{"localhost:8080"};

    // Display constants (SSOT for widget appearance)
    static constexpr int INDICATOR_SIZE = 16;
    static constexpr int WIDGET_SPACING = 8;
    static constexpr int WIDGET_MARGIN = 4;
    static constexpr const char* DISCONNECTED_TEXT = "Disconnected";
    static constexpr const char* CONNECTING_TEXT = "Connecting...";
    static constexpr const char* CONNECTED_TEXT = "Connected";
    static constexpr const char* DISCONNECTED_COLOR = "#FF0000";  // Red
    static constexpr const char* CONNECTING_COLOR = "#FF4500";    // Orange-red
    static constexpr const char* CONNECTED_COLOR = "#00FF00";     // Green
};

} // namespace ui
} // namespace siren

#endif // SIREN_CONNECTION_STATUS_WIDGET_H