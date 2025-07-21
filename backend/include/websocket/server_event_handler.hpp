/**
 * @file server_event_handler.hpp
 * @brief WebSocket server event handling - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Handle WebSocket server events ONLY
 *
 * RESPONSIBILITIES:
 * - Process connection accepted events
 * - Handle connection errors
 * - Process session lifecycle events
 * - Handle broadcast completion events
 *
 * NOT RESPONSIBLE FOR:
 * - Server lifecycle (handled by WebSocketServer)
 * - Component coordination (handled by WebSocketServer)
 * - Session management (handled by SessionManager)
 * - Message broadcasting (handled by MessageBroadcaster)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */

#pragma once

#include <memory>
#include <string>
#include <functional>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "data/sonar_types.hpp"

namespace siren::websocket {

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// Forward declarations
class WebSocketSession;
class SessionManager;
class StatisticsCollector;

/**
 * @brief WebSocket server event handler with single responsibility
 *
 * Handles all event processing for the WebSocket server without
 * mixing concerns with server lifecycle or component coordination.
 *
 * Single Responsibility: WebSocket server event processing
 */
class ServerEventHandler {
public:
    /// Connection callback type (SSOT)
    using ConnectionCallback = std::function<void(const std::string&, bool)>;

    /**
     * @brief Constructor - RAII initialization
     * @param session_manager Reference to session manager for session operations
     * @param statistics_collector Reference to statistics collector for metrics
     */
    explicit ServerEventHandler(std::shared_ptr<SessionManager> session_manager,
                               std::shared_ptr<StatisticsCollector> statistics_collector);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~ServerEventHandler() = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    ServerEventHandler(const ServerEventHandler&) = delete;
    ServerEventHandler& operator=(const ServerEventHandler&) = delete;
    ServerEventHandler(ServerEventHandler&&) = delete;
    ServerEventHandler& operator=(ServerEventHandler&&) = delete;

    /**
     * @brief Handle accepted connection event (SSOT for connection acceptance)
     * @param socket Accepted TCP socket
     * @param server_weak_ptr Weak reference to server for session creation
     */
    void onConnectionAccepted(tcp::socket socket,
                             std::weak_ptr<class WebSocketServer> server_weak_ptr);

    /**
     * @brief Handle connection error event (SSOT for connection errors)
     * @param error_message Error description
     * @param ec Beast error code
     */
    void onConnectionError(const std::string& error_message, beast::error_code ec);

    /**
     * @brief Handle session event (SSOT for session lifecycle events)
     * @param endpoint Client endpoint
     * @param connected true if connected, false if disconnected
     */
    void onSessionEvent(const std::string& endpoint, bool connected);

    /**
     * @brief Handle broadcast completion event (SSOT for broadcast events)
     * @param sessions_reached Number of sessions reached in broadcast
     */
    void onBroadcastCompleted(size_t sessions_reached);

    /**
     * @brief Set connection callback for external notification
     * @param callback Function to call on connection events
     */
    void setConnectionCallback(ConnectionCallback callback);

    /**
     * @brief Get active connection count for logging
     */
    size_t getActiveConnections() const;

private:
    // Component references - not owned, avoid circular dependencies
    std::weak_ptr<SessionManager> session_manager_;
    std::weak_ptr<StatisticsCollector> statistics_collector_;

    // Event callback - SSOT for external notification
    ConnectionCallback connection_callback_;
};

} // namespace siren::websocket