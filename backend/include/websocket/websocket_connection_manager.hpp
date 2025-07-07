/**
 * @file websocket_connection_manager.hpp
 * @brief Military-grade WebSocket connection manager for TCP connection acceptance
 * @author unoRadar Project
 * @date 2025
 *
 * Specialized manager class responsible for handling TCP connection acceptance
 * and initial WebSocket handshake coordination. Follows SRP by focusing
 * exclusively on connection lifecycle management.
 */

#pragma once

#include <memory>
#include <functional>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace unoradar::websocket {

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// Forward declarations
class WebSocketSession;
class WebSocketServer;

/**
 * @brief Military-grade WebSocket connection manager
 *
 * Handles TCP connection acceptance and initial handshake coordination.
 * Focuses exclusively on connection lifecycle management following SRP.
 *
 * Features:
 * - Asynchronous TCP connection acceptance
 * - Clean connection lifecycle management
 * - Military-grade error handling
 * - Thread-safe operations
 * - Graceful shutdown support
 */
class WebSocketConnectionManager {
public:
    /// Connection acceptance callback type
    using AcceptCallback = std::function<void(tcp::socket)>;

    /// Error callback type
    using ErrorCallback = std::function<void(const std::string&, beast::error_code)>;

    /**
     * @brief Constructor
     * @param io_context Boost.Asio I/O context
     * @param port TCP port for incoming connections
     */
    explicit WebSocketConnectionManager(boost::asio::io_context& io_context, uint16_t port);

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~WebSocketConnectionManager();

    // Non-copyable, non-movable
    WebSocketConnectionManager(const WebSocketConnectionManager&) = delete;
    WebSocketConnectionManager& operator=(const WebSocketConnectionManager&) = delete;
    WebSocketConnectionManager(WebSocketConnectionManager&&) = delete;
    WebSocketConnectionManager& operator=(WebSocketConnectionManager&&) = delete;

    /**
     * @brief Initialize the connection manager
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start accepting connections
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop accepting connections gracefully
     */
    void stop();

    /**
     * @brief Check if connection manager is active
     */
    bool isActive() const noexcept;

    /**
     * @brief Get the listening port
     */
    uint16_t getPort() const noexcept;

    /**
     * @brief Set callback for accepted connections
     * @param callback Function to call when connection accepted
     */
    void setAcceptCallback(AcceptCallback callback);

    /**
     * @brief Set callback for connection errors
     * @param callback Function to call when errors occur
     */
    void setErrorCallback(ErrorCallback callback);

private:
    // Core components
    boost::asio::io_context& io_context_;
    std::unique_ptr<tcp::acceptor> acceptor_;
    uint16_t port_;

    // State management
    std::atomic<bool> active_;
    std::atomic<bool> shutdown_requested_;

    // Callbacks
    AcceptCallback accept_callback_;
    ErrorCallback error_callback_;

    /**
     * @brief Start asynchronous accept operation
     */
    void startAccept();

    /**
     * @brief Handle accepted connection
     * @param ec Error code from accept operation
     * @param socket Accepted TCP socket
     */
    void onAccept(beast::error_code ec, tcp::socket socket);

    /**
     * @brief Handle connection manager errors
     * @param error_message Descriptive error message
     * @param ec Boost.Beast error code
     */
    void handleError(const std::string& error_message, beast::error_code ec);
};

} // namespace unoradar::websocket