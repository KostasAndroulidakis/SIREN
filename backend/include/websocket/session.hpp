/**
 * @file session.hpp
 * @brief WebSocket session for individual client connections - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Handle WebSocket protocol communication for one client
 * 
 * RESPONSIBILITIES:
 * - WebSocket protocol handling (handshake, read, write)
 * - Message serialization and transmission
 * - Connection state management for single client
 * - Client endpoint information
 *
 * NOT RESPONSIBLE FOR:
 * - Session lifecycle management (handled by SessionManager)
 * - Message broadcasting (handled by MessageBroadcaster) 
 * - Connection acceptance (handled by ConnectionAcceptor)
 * - Server orchestration (handled by WebSocketServer)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */

#pragma once

#include <memory>
#include <string>
#include <atomic>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>

#include "data/sonar_types.hpp"

namespace siren::websocket {

namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

// Forward declaration to avoid circular dependency
class WebSocketServer;

/**
 * @brief WebSocket session for individual clients with single responsibility
 *
 * Each client connection is handled by a separate session instance
 * with military-grade error handling and performance monitoring.
 *
 * Single Responsibility: WebSocket protocol communication for one client
 */
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    /**
     * @brief Constructor - RAII initialization
     * @param socket TCP socket for the connection
     * @param server_weak_ptr Weak reference to parent server
     */
    explicit WebSocketSession(tcp::socket&& socket,
                             std::weak_ptr<WebSocketServer> server_weak_ptr);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~WebSocketSession() = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    WebSocketSession(const WebSocketSession&) = delete;
    WebSocketSession& operator=(const WebSocketSession&) = delete;
    WebSocketSession(WebSocketSession&&) = delete;
    WebSocketSession& operator=(WebSocketSession&&) = delete;

    /**
     * @brief Start the WebSocket session
     */
    void start();

    /**
     * @brief Send sonar data to client
     * @param data Sonar data point to send
     */
    void sendSonarData(const data::SonarDataPoint& data);

    /**
     * @brief Send performance metrics to client
     * @param metrics Performance metrics to send
     */
    void sendPerformanceMetrics(const data::PerformanceMetrics& metrics);

    /**
     * @brief Send generic message to client
     * @param message Serialized message to send
     */
    void sendMessage(const std::string& message);

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
    // WebSocket stream - RAII managed
    websocket::stream<beast::tcp_stream> ws_;
    
    // Server reference - weak to avoid circular dependency
    std::weak_ptr<WebSocketServer> server_weak_ptr_;
    
    // Client information - SSOT
    std::string client_endpoint_;
    
    // Connection state - atomic for thread safety
    std::atomic<bool> is_alive_;
    std::atomic<bool> closing_;

    // Message queue for thread-safe sending - SSOT for message ordering
    std::queue<std::string> message_queue_;
    mutable std::mutex queue_mutex_;
    std::atomic<bool> write_in_progress_;

    // Read buffer - RAII managed
    beast::flat_buffer buffer_;

    /**
     * @brief Handle WebSocket handshake (SSOT for handshake logic)
     * @param ec Error code from handshake operation
     */
    void onAccept(beast::error_code ec);

    /**
     * @brief Handle incoming messages (SSOT for read logic)
     * @param ec Error code from read operation
     * @param bytes_transferred Number of bytes read
     */
    void onRead(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Handle outgoing message writes (SSOT for write logic)
     * @param ec Error code from write operation
     * @param bytes_transferred Number of bytes written
     */
    void onWrite(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Process next message in queue (SSOT for message processing)
     */
    void processNextMessage();

    /**
     * @brief Enqueue message for sending (SSOT for message queuing)
     * @param message Serialized message to send
     */
    void enqueueMessage(const std::string& message);

    /**
     * @brief Handle connection errors (SSOT for error handling)
     * @param error_message Error description
     * @param ec Beast error code
     */
    void handleError(const std::string& error_message, beast::error_code ec);
};

} // namespace siren::websocket