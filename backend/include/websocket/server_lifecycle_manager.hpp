/**
 * @file server_lifecycle_manager.hpp
 * @brief WebSocket server lifecycle management - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Server component initialization, startup, and shutdown ONLY
 *
 * RESPONSIBILITIES:
 * - Initialize all server components in correct order
 * - Start all server components with error rollback
 * - Stop all server components gracefully
 * - Coordinate component state transitions
 *
 * NOT RESPONSIBLE FOR:
 * - Event handling (handled by ServerEventHandler)
 * - Session management (handled by SessionManager)
 * - Message broadcasting (handled by MessageBroadcaster)
 * - Connection acceptance (handled by ConnectionAcceptor)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */

#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "data/sonar_types.hpp"

namespace siren::websocket {

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// Forward declarations
class ConnectionAcceptor;
class SessionManager;
class MessageBroadcaster;
class StatisticsCollector;
class ServerEventHandler;
class WebSocketServer;

/**
 * @brief WebSocket server lifecycle manager with single responsibility
 *
 * Handles initialization, startup, and shutdown of server components without
 * mixing concerns with event handling or message processing.
 *
 * Single Responsibility: Server component lifecycle management
 */
class ServerLifecycleManager {
public:
    /**
     * @brief Constructor - RAII initialization
     * @param connection_acceptor Reference to connection acceptor
     * @param session_manager Reference to session manager
     * @param message_broadcaster Reference to message broadcaster
     * @param statistics_collector Reference to statistics collector
     * @param event_handler Reference to event handler
     * @param port Server port for logging
     */
    explicit ServerLifecycleManager(
        std::unique_ptr<ConnectionAcceptor>& connection_acceptor,
        std::shared_ptr<SessionManager>& session_manager,
        std::unique_ptr<MessageBroadcaster>& message_broadcaster,
        std::shared_ptr<StatisticsCollector>& statistics_collector,
        std::unique_ptr<ServerEventHandler>& event_handler,
        uint16_t port);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~ServerLifecycleManager() = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    ServerLifecycleManager(const ServerLifecycleManager&) = delete;
    ServerLifecycleManager& operator=(const ServerLifecycleManager&) = delete;
    ServerLifecycleManager(ServerLifecycleManager&&) = delete;
    ServerLifecycleManager& operator=(ServerLifecycleManager&&) = delete;

    /**
     * @brief Initialize all server components (SSOT for initialization)
     * @param server_weak_ptr Weak reference to server for callback setup
     * @return true if all components initialized successfully
     */
    bool initialize(std::weak_ptr<WebSocketServer> server_weak_ptr);

    /**
     * @brief Start all server components (SSOT for startup)
     * @param running Reference to server running state
     * @return true if all components started successfully
     */
    bool start(std::atomic<bool>& running);

    /**
     * @brief Stop all server components (SSOT for shutdown)
     * @param running Reference to server running state
     * @param shutdown_requested Reference to shutdown request flag
     */
    void stop(std::atomic<bool>& running, std::atomic<bool>& shutdown_requested);

private:
    // Component references - not owned, avoid circular dependencies
    std::unique_ptr<ConnectionAcceptor>& connection_acceptor_;
    std::shared_ptr<SessionManager>& session_manager_;
    std::unique_ptr<MessageBroadcaster>& message_broadcaster_;
    std::shared_ptr<StatisticsCollector>& statistics_collector_;
    std::unique_ptr<ServerEventHandler>& event_handler_;

    // Server configuration
    uint16_t port_;

    /**
     * @brief Rollback started components on failure (SSOT for rollback logic)
     * @param connection_acceptor_started true if connection acceptor was started
     * @param message_broadcaster_started true if message broadcaster was started
     * @param statistics_collector_started true if statistics collector was started
     */
    void rollbackStartedComponents(bool connection_acceptor_started,
                                  bool message_broadcaster_started,
                                  bool statistics_collector_started) noexcept;
};

} // namespace siren::websocket