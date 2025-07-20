/**
 * CLASSIFICATION: UNCLASSIFIED
 * EXPORT CONTROL: NOT SUBJECT TO EAR/ITAR
 * CONTRACT: SIREN-2025
 *
 * @file WebSocketClient.h
 * @brief Concrete WebSocket client implementation with automatic reconnection
 * @author SIREN Defense Systems
 * @date 2025
 *
 * MISRA C++ 2008 Compliant
 * DO-178C Level A Certifiable
 */

#ifndef SIREN_NETWORK_WEBSOCKET_CLIENT_H
#define SIREN_NETWORK_WEBSOCKET_CLIENT_H

#include "IWebSocketClient.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtWebSockets/QWebSocket>

namespace siren {
namespace Network {

/**
 * @brief Military-grade WebSocket client with automatic reconnection
 *
 * Implements exponential backoff and connection state management
 * MISRA C++ 2008: 0-1-11 - All parameters used
 */
class WebSocketClient final : public IWebSocketClient {
    Q_OBJECT
    Q_DISABLE_COPY(WebSocketClient)

public:
    explicit WebSocketClient(QObject* parent = nullptr);
    ~WebSocketClient() override = default;

    // IWebSocketClient interface implementation
    void connectToServer(const QUrl& url) override;
    void disconnectFromServer() override;
    bool isConnected() const override;
    void sendTextMessage(const QString& message) override;
    void sendBinaryMessage(const QByteArray& data) override;
    State state() const override;

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);
    void attemptReconnect();

private:
    // Private implementation methods
    void resetReconnectAttempts();
    std::int32_t calculateReconnectDelay() const;

    // Member variables - MISRA C++ 2008: 11-0-1 - Member data private
    QScopedPointer<QWebSocket> m_webSocket;
    QScopedPointer<QTimer> m_reconnectTimer;
    QUrl m_serverUrl;
    std::int32_t m_reconnectAttempts;
    bool m_autoReconnect;
    State m_state;
};

} // namespace Network
} // namespace siren

#endif // SIREN_NETWORK_WEBSOCKET_CLIENT_H