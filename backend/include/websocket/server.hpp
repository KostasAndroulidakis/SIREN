/**
 * @file server.hpp
 * @brief Military-grade WebSocket server for real-time sonar data streaming
 * @author SIREN Project
 * @date 2025
 *
 * High-performance WebSocket server using Boost.Beast for real-time
 * sonar data distribution to frontend clients.
 */

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

#include "data/sonar_types.hpp"
#include "websocket/connection_acceptor.hpp"
#include "websocket/session_manager.hpp"
#include "websocket/message_broadcaster.hpp"
#include "websocket/statistics_collector.hpp"
#include "websocket/session.hpp"

namespace siren::websocket {

namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

// WebSocketSession class now in separate file: websocket/session.hpp

/**
 * @brief Military-grade WebSocket server for sonar data streaming
 *
 * Features:
 * - Real-time sonar data broadcasting
 * - Client connection management
 * - Performance monitoring
 * - Error recovery
 * - Military-grade security and reliability
 */
class WebSocketServer : public std::enable_shared_from_this<WebSocketServer> {
public:
    /// Client connection callback type
    using ConnectionCallback = std::function<void(const std::string&, bool)>;

    /**
     * @brief Constructor
     * @param io_context Boost.Asio I/O context
     * @param port WebSocket server port
     */
    explicit WebSocketServer(boost::asio::io_context& io_context, uint16_t port);

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~WebSocketServer();

    // Non-copyable, non-movable
    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;
    WebSocketServer(WebSocketServer&&) = delete;
    WebSocketServer& operator=(WebSocketServer&&) = delete;

    /**
     * @brief Initialize WebSocket server
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start accepting connections
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop the server gracefully
     */
    void stop();

    /**
     * @brief Check if server is running
     */
    bool isRunning() const noexcept;

    /**
     * @brief Broadcast sonar data to all connected clients
     * @param data Sonar data point to broadcast
     */
    void broadcastSonarData(const data::SonarDataPoint& data);

    /**
     * @brief Broadcast performance metrics to all connected clients
     * @param metrics Performance metrics to broadcast
     */
    void broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics);

    /**
     * @brief Get number of active connections
     */
    size_t getActiveConnections() const noexcept;

    /**
     * @brief Get server statistics
     */
    data::WebSocketStatistics getStatistics() const;

    /**
     * @brief Set connection callback
     * @param callback Function to call when clients connect/disconnect
     */
    void setConnectionCallback(ConnectionCallback callback);

    /**
     * @brief Remove session from active connections (called by sessions)
     */
    void removeSession(std::shared_ptr<WebSocketSession> session);

private:
    // Core components
    boost::asio::io_context& io_context_;
    uint16_t port_;

    // State management
    std::atomic<bool> running_;
    std::atomic<bool> shutdown_requested_;

    // Specialized managers - SRP compliant components
    std::unique_ptr<ConnectionAcceptor> connection_acceptor_;
    std::unique_ptr<SessionManager> session_manager_;
    std::unique_ptr<MessageBroadcaster> message_broadcaster_;
    std::unique_ptr<StatisticsCollector> statistics_collector_;

    // Callbacks
    ConnectionCallback connection_callback_;

    /**
     * @brief Handle accepted connection from connection acceptor
     * @param socket Accepted TCP socket
     */
    void onConnectionAccepted(tcp::socket socket);

    /**
     * @brief Handle connection acceptor errors
     * @param error_message Error description
     * @param ec Error code
     */
    void onConnectionError(const std::string& error_message, beast::error_code ec);

    /**
     * @brief Handle session connection events
     * @param endpoint Client endpoint
     * @param connected true if connected, false if disconnected
     */
    void onSessionEvent(const std::string& endpoint, bool connected);

    /**
     * @brief Handle broadcast completion events
     * @param sessions_reached Number of sessions reached in broadcast
     */
    void onBroadcastCompleted(size_t sessions_reached);

};

} // namespace siren::websocket