// SIREN Military-Grade Radar System
// WebSocket Client Implementation
// Single Responsibility: Backend Communication ONLY

#include "network/WebSocketClient.h"
#include <QWebSocket>
#include <QTimer>
#include <QUrl>
#include <QDebug>

namespace siren {
namespace Network {

// SSOT for timing constants (MISRA C++ Rule 5.0.1: No magic numbers)
namespace {
    constexpr int BASE_RECONNECT_DELAY_MS = 1000;      // 1 second
    constexpr int MAX_RECONNECT_DELAY_MS = 30000;      // 30 seconds
    constexpr int MAX_RECONNECT_ATTEMPTS = 10;         // Maximum retry attempts
    constexpr int RECONNECT_BACKOFF_MULTIPLIER = 2;    // Exponential backoff
}

WebSocketClient::WebSocketClient(QObject* parent)
    : IWebSocketClient(parent)
    , m_webSocket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
    , m_reconnectTimer(new QTimer(this))
    , m_serverUrl()
    , m_reconnectAttempts(0)
    , m_autoReconnect(true)
    , m_state(State::Disconnected)
{
    // Configure reconnect timer (single-shot)
    m_reconnectTimer->setSingleShot(true);
    
    // Connect WebSocket signals (MISRA C++ Rule 21.2.1: RAII for connections)
    connect(m_webSocket.data(), &QWebSocket::connected, 
            this, &WebSocketClient::onConnected);
    connect(m_webSocket.data(), &QWebSocket::disconnected, 
            this, &WebSocketClient::onDisconnected);
    connect(m_webSocket.data(), &QWebSocket::textMessageReceived, 
            this, &WebSocketClient::onTextMessageReceived);
    connect(m_webSocket.data(), &QWebSocket::errorOccurred,
            this, &WebSocketClient::onError);
    
    // Connect reconnect timer
    connect(m_reconnectTimer.data(), &QTimer::timeout, 
            this, &WebSocketClient::attemptReconnect);
}

void WebSocketClient::connectToServer(const QUrl& url)
{
    // MISRA C++ Rule 8.18.2: No assignment in conditions
    if (m_state == State::Connected || m_state == State::Connecting) {
        return; // Already connected or connecting
    }
    
    m_serverUrl = url;
    m_state = State::Connecting;
    resetReconnectAttempts();
    
    emit stateChanged(m_state);
    
    // Start connection attempt
    m_webSocket->open(m_serverUrl);
}

void WebSocketClient::disconnectFromServer()
{
    // Stop any reconnection attempts
    m_reconnectTimer->stop();
    m_autoReconnect = false;
    
    if (m_state == State::Connected || m_state == State::Connecting) {
        m_state = State::Closing;
        emit stateChanged(m_state);
        m_webSocket->close();
    }
}

bool WebSocketClient::isConnected() const
{
    return (m_state == State::Connected);
}

void WebSocketClient::sendTextMessage(const QString& message)
{
    if (m_state == State::Connected) {
        const qint64 bytesSent = m_webSocket->sendTextMessage(message);
        if (bytesSent == -1) {
            emit errorOccurred("Failed to send text message");
        }
    } else {
        emit errorOccurred("Cannot send message: not connected");
    }
}

void WebSocketClient::sendBinaryMessage(const QByteArray& data)
{
    if (m_state == State::Connected) {
        const qint64 bytesSent = m_webSocket->sendBinaryMessage(data);
        if (bytesSent == -1) {
            emit errorOccurred("Failed to send binary message");
        }
    } else {
        emit errorOccurred("Cannot send message: not connected");
    }
}

WebSocketClient::State WebSocketClient::state() const
{
    return m_state;
}

void WebSocketClient::onConnected()
{
    m_state = State::Connected;
    resetReconnectAttempts();
    
    emit connected();
    emit stateChanged(m_state);
    
    qDebug() << "WebSocket connected to:" << m_serverUrl.toString();
}

void WebSocketClient::onDisconnected()
{
    const State previousState = m_state;
    m_state = State::Disconnected;
    
    emit disconnected();
    emit stateChanged(m_state);
    
    qDebug() << "WebSocket disconnected from:" << m_serverUrl.toString();
    
    // Only attempt reconnection if we were previously connected and auto-reconnect is enabled
    if (m_autoReconnect && previousState == State::Connected) {
        const int delay = calculateReconnectDelay();
        m_reconnectTimer->start(delay);
        emit reconnectScheduled(m_reconnectAttempts + 1, delay);
    }
}

void WebSocketClient::onTextMessageReceived(const QString& message)
{
    emit textMessageReceived(message);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    QString errorString;
    
    // Convert socket error to string (MISRA C++ Rule 16.1.1: All cases covered)
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            errorString = "Connection refused";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            errorString = "Remote host closed connection";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorString = "Host not found";
            break;
        case QAbstractSocket::SocketTimeoutError:
            errorString = "Connection timeout";
            break;
        case QAbstractSocket::NetworkError:
            errorString = "Network error";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            errorString = "Proxy authentication required";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            errorString = "SSL handshake failed";
            break;
        default:
            errorString = QString("Socket error: %1").arg(static_cast<int>(error));
            break;
    }
    
    emit errorOccurred(errorString);
    
    // If we're currently connecting and get an error, attempt reconnection
    if (m_state == State::Connecting && m_autoReconnect) {
        const int delay = calculateReconnectDelay();
        m_reconnectTimer->start(delay);
        emit reconnectScheduled(m_reconnectAttempts + 1, delay);
    }
    
    m_state = State::Disconnected;
    emit stateChanged(m_state);
}

void WebSocketClient::attemptReconnect()
{
    // MISRA C++ Rule 8.18.2: No assignment in conditions
    m_reconnectAttempts++;
    
    if (m_reconnectAttempts > MAX_RECONNECT_ATTEMPTS) {
        emit errorOccurred("Maximum reconnection attempts exceeded");
        return;
    }
    
    qDebug() << "Attempting reconnection #" << m_reconnectAttempts << "to:" << m_serverUrl.toString();
    
    m_state = State::Connecting;
    emit stateChanged(m_state);
    
    m_webSocket->open(m_serverUrl);
}

void WebSocketClient::resetReconnectAttempts()
{
    m_reconnectAttempts = 0;
    m_autoReconnect = true;
}

std::int32_t WebSocketClient::calculateReconnectDelay() const
{
    // Exponential backoff: delay = base * (multiplier ^ attempts)
    // MISRA C++ Rule 5.0.1: Use constants instead of magic numbers
    std::int32_t delay = BASE_RECONNECT_DELAY_MS;
    
    for (int i = 0; i < m_reconnectAttempts && delay < MAX_RECONNECT_DELAY_MS; ++i) {
        delay *= RECONNECT_BACKOFF_MULTIPLIER;
    }
    
    // Cap at maximum delay
    if (delay > MAX_RECONNECT_DELAY_MS) {
        delay = MAX_RECONNECT_DELAY_MS;
    }
    
    return delay;
}

} // namespace Network
} // namespace siren