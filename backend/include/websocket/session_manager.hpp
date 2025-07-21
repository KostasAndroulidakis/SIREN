/**
 * @file session_manager.hpp
 * @brief WebSocket session lifecycle manager - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Manage WebSocket session lifecycle ONLY
 * MISRA C++ 2023 compliant - Rule 5.0.1 (no magic numbers), Rule 21.2.1 (RAII)
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace siren::websocket {

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// Forward declaration
class WebSocketSession;
class WebSocketServer;

/**
 * @brief WebSocket session lifecycle manager with single responsibility
 *
 * RESPONSIBILITIES:
 * - Create WebSocket sessions from TCP sockets ONLY
 * - Track active session lifecycle ONLY
 * - Cleanup closed sessions ONLY
 * - Provide session access for broadcasting ONLY
 *
 * NOT RESPONSIBLE FOR:
 * - TCP connection acceptance (handled by ConnectionAcceptor)
 * - Message broadcasting (handled by MessageBroadcaster)
 * - Statistics tracking (handled by StatisticsCollector)
 * - Server coordination (handled by WebSocketServer)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 * - Rule 18.1.1: Thread-safe container access
 */
class SessionManager {
public:
    /// Session container type (SSOT for session storage)
    using SessionContainer = std::vector<std::shared_ptr<WebSocketSession>>;
    
    /// Callback type for session events (SSOT)
    using SessionEventCallback = std::function<void(const std::string&, bool)>;

    /**
     * @brief Constructor - RAII initialization
     */
    explicit SessionManager();

    /**
     * @brief Destructor - RAII cleanup
     */
    ~SessionManager();

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;
    SessionManager(SessionManager&&) = delete;
    SessionManager& operator=(SessionManager&&) = delete;

    /**
     * @brief Create session from accepted TCP socket (SSOT for session creation)
     * @param socket Accepted TCP socket
     * @param server_weak_ptr Weak reference to parent server
     * @return Shared pointer to created session
     */
    std::shared_ptr<WebSocketSession> createSession(tcp::socket&& socket, 
                                                   std::weak_ptr<WebSocketServer> server_weak_ptr);

    /**
     * @brief Remove session from active sessions (SSOT for session removal)
     * @param session Session to remove
     */
    void removeSession(std::shared_ptr<WebSocketSession> session);

    /**
     * @brief Get active sessions for broadcasting (SSOT for session access)
     * @return Container of active sessions
     */
    SessionContainer getActiveSessions() const;

    /**
     * @brief Get count of active sessions (SSOT for session counting)
     * @return Number of active sessions
     */
    size_t getActiveSessionCount() const noexcept;

    /**
     * @brief Close all sessions gracefully (SSOT for bulk session closure)
     */
    void closeAllSessions();

    /**
     * @brief Cleanup closed sessions from container (SSOT for cleanup)
     */
    void cleanupClosedSessions();

    /**
     * @brief Set callback for session events (SSOT for callback setting)
     * @param callback Function to call when session connects/disconnects
     */
    void setSessionCallback(SessionEventCallback callback);

private:
    // Session storage - thread-safe access required
    mutable std::mutex sessions_mutex_;
    SessionContainer active_sessions_;

    // Session event notification (SSOT)
    SessionEventCallback session_callback_;

    // Cleanup management
    std::atomic<size_t> cleanup_counter_;

    // SSOT constants for cleanup management (MISRA C++ Rule 5.0.1)
    static constexpr size_t CLEANUP_THRESHOLD = 10;  // Cleanup after N session changes
    static constexpr const char* COMPONENT_NAME = "SessionManager";

    /**
     * @brief Notify session event (SSOT for event notification)
     * @param endpoint Client endpoint
     * @param connected true if connected, false if disconnected
     */
    void notifySessionEvent(const std::string& endpoint, bool connected);

    /**
     * @brief Check if periodic cleanup is needed (SSOT for cleanup logic)
     */
    void checkPeriodicCleanup();
};

} // namespace siren::websocket