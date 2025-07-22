/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file SonarMainWindow.h
 * @brief Main application window for SIREN system
 * @author KostasAndroulidakis
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_UI_SONAR_MAIN_WINDOW_H
#define SIREN_UI_SONAR_MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QScopedPointer>
#include <memory>

// Forward declarations - MISRA C++ 2008: 2-7-1
QT_BEGIN_NAMESPACE
class QTextEdit;
class QStatusBar;
QT_END_NAMESPACE

namespace siren {

// Forward declarations
namespace Network { class IWebSocketClient; }
namespace Data { class SonarDataParser; struct SonarData; }

namespace UI {

/**
 * @brief Main application window
 *
 * Single Responsibility: UI management and coordination only
 * MISRA C++ 2008: 11-0-1 - Member data private
 */
class SonarMainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(SonarMainWindow)

public:
    explicit SonarMainWindow(QWidget* parent = nullptr);
    ~SonarMainWindow() override;

    // Dependency injection for testability
    void setWebSocketClient(std::unique_ptr<Network::IWebSocketClient> client);
    void setDataParser(std::unique_ptr<Data::SonarDataParser> parser);

signals:
    void connectRequested();
    void disconnectRequested();

private slots:
    // WebSocket events
    void onConnectionEstablished();
    void onConnectionLost();
    void onMessageReceived(const QString& message);
    void onConnectionError(const QString& error);

    // Data events
    void onDataParsed(const Data::SonarData& data);
    void onParseError(const QString& error);

private:
    // UI Setup - Single Responsibility methods
    void setupUi();
    void setupMenuBar();
    void setupCentralWidget();
    void setupStatusBar();
    void applyTheme();

    // UI Update methods
    void updateConnectionStatus(bool connected);
    void displaySonarData(const Data::SonarData& data);
    void showError(const QString& error);

    // Member variables - MISRA C++ 2008: 11-0-1
    QScopedPointer<QTextEdit> m_dataDisplay;
    std::unique_ptr<Network::IWebSocketClient> m_webSocketClient;
    std::unique_ptr<Data::SonarDataParser> m_dataParser;

    // UI state
    bool m_isConnected;
    std::int32_t m_messageCount;
};

} // namespace UI
} // namespace siren

#endif // SIREN_UI_SONAR_MAIN_WINDOW_H