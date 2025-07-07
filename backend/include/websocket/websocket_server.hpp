/**
 * @file websocket_server.hpp
 * @brief Military-grade WebSocket server for real-time radar data streaming
 * @author unoRadar Project
 * @date 2025
 *
 * High-performance WebSocket server using Boost.Beast for real-time
 * radar data distribution to frontend clients.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

#include "data/radar_types.hpp"

namespace unoradar::websocket {

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

/**
 * @brief WebSocket connection session for individual clients
 *
 * Each client connection is handled by a separate session instance
 * with military-grade error handling and performance monitoring.
 */
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    /**
     * @brief Constructor
     * @param socket TCP socket for the connection
     * @param server_weak_ptr Weak reference to parent server
     */
    explicit WebSocketSession(tcp::socket&& socket,
                             std::weak_ptr<class WebSocketServer> server_weak_ptr);

    /**
     * @brief Start the WebSocket session
     */
    void start();

    /**
     * @brief Send radar data to client
     * @param data Radar data point to send
     */
    void sendRadarData(const data::RadarDataPoint& data);

    /**
     * @brief Send performance metrics to client
     * @param metrics Performance metrics to send
     */
    void sendPerformanceMetrics(const data::PerformanceMetrics& metrics);

    /**
     * @brief Close the connection gracefully
     */
    void close();

    /**
     * @brief Check if connection is alive
     */
    bool isAlive() const noexcept;

    /**
     * @brief Get client endpoint information
     */
    std::string getClientEndpoint() const;

private:
    websocket::stream<beast::tcp_stream> ws_;
    std::weak_ptr<WebSocketServer> server_weak_ptr_;
    std::string client_endpoint_;
    std::atomic<bool> is_alive_;
    std::atomic<bool> closing_;

    // Message queue for thread-safe sending
    std::queue<std::string> message_queue_;
    std::mutex queue_mutex_;
    std::atomic<bool> write_in_progress_;

    /**
     * @brief Handle WebSocket handshake
     */
    void onAccept(beast::error_code ec);

    /**
     * @brief Handle incoming messages
     */
    void onRead(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Handle outgoing message writes
     */
    void onWrite(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Process next message in queue
     */
    void processNextMessage();

    /**
     * @brief Handle connection errors
     */
    void handleError(const std::string& error_message, beast::error_code ec);

    /**
     * @brief Serialize radar data to JSON
     */
    std::string serializeRadarData(const data::RadarDataPoint& data);

    /**
     * @brief Serialize performance metrics to JSON
     */
    std::string serializePerformanceMetrics(const data::PerformanceMetrics& metrics);

    beast::flat_buffer buffer_;
};

/**
 * @brief Military-grade WebSocket server for radar data streaming
 *
 * Features:
 * - Real-time radar data broadcasting
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
     * @brief Broadcast radar data to all connected clients
     * @param data Radar data point to broadcast
     */
    void broadcastRadarData(const data::RadarDataPoint& data);

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
    std::unique_ptr<tcp::acceptor> acceptor_;
    uint16_t port_;

    // State management
    std::atomic<bool> running_;
    std::atomic<bool> shutdown_requested_;

    // Connection management
    std::unordered_set<std::shared_ptr<WebSocketSession>> active_sessions_;
    mutable std::mutex sessions_mutex_;

    // Statistics and monitoring
    mutable std::mutex stats_mutex_;
    data::WebSocketStatistics statistics_;
    std::chrono::steady_clock::time_point server_start_time_;

    // Callbacks
    ConnectionCallback connection_callback_;

    /**
     * @brief Start accepting incoming connections
     */
    void startAccept();

    /**
     * @brief Handle new connection
     * @param ec Error code from accept operation
     * @param socket New client socket
     */
    void onAccept(beast::error_code ec, tcp::socket socket);

    /**
     * @brief Add new session to active connections
     * @param session New WebSocket session
     */
    void addSession(std::shared_ptr<WebSocketSession> session);

    /**
     * @brief Clean up closed sessions
     */
    void cleanupClosedSessions();

    /**
     * @brief Update server statistics
     */
    void updateStatistics();

    /**
     * @brief Handle server errors
     */
    void handleError(const std::string& error_message, beast::error_code ec);
};

} // namespace unoradar::websocket