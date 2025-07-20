/**
 * @file session_manager.hpp
 * @brief Military-grade WebSocket session manager for active session lifecycle
 * @author unoRadar Project
 * @date 2025
 *
 * Specialized manager class responsible for managing active WebSocket sessions.
 * Follows SRP by focusing exclusively on session lifecycle management including
 * session creation, tracking, health monitoring, and cleanup.
 */

#pragma once

#include <memory>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace siren::websocket {

using tcp = boost::asio::ip::tcp;

// Forward declarations
class WebSocketSession;
class WebSocketServer;

/**
 * @brief Military-grade WebSocket session manager
 *
 * Manages active WebSocket sessions including creation, registration,
 * health monitoring, and cleanup. Follows SRP by focusing exclusively
 * on session lifecycle management.
 *
 * Features:
 * - Thread-safe session management
 * - Automatic session cleanup
 * - Session health monitoring
 * - Military-grade error handling
 * - Connection lifecycle callbacks
 * - Graceful session shutdown
 */
class WebSocketSessionManager {
public:
    /// Session callback type for connection events
    using SessionCallback = std::function<void(const std::string&, bool)>;

    /**
     * @brief Constructor
     */
    explicit WebSocketSessionManager();

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~WebSocketSessionManager();

    // Non-copyable, non-movable
    WebSocketSessionManager(const WebSocketSessionManager&) = delete;
    WebSocketSessionManager& operator=(const WebSocketSessionManager&) = delete;
    WebSocketSessionManager(WebSocketSessionManager&&) = delete;
    WebSocketSessionManager& operator=(WebSocketSessionManager&&) = delete;

    /**
     * @brief Create and register a new WebSocket session
     * @param socket TCP socket for the session
     * @param server_weak_ptr Weak reference to parent server
     * @return Shared pointer to the new session
     */
    std::shared_ptr<WebSocketSession> createSession(tcp::socket&& socket,
                                                   std::weak_ptr<WebSocketServer> server_weak_ptr);

    /**
     * @brief Add existing session to management
     * @param session WebSocket session to manage
     */
    void addSession(std::shared_ptr<WebSocketSession> session);

    /**
     * @brief Remove session from management
     * @param session WebSocket session to remove
     */
    void removeSession(std::shared_ptr<WebSocketSession> session);

    /**
     * @brief Get all active sessions
     * @return Set of active session pointers
     */
    std::unordered_set<std::shared_ptr<WebSocketSession>> getActiveSessions() const;

    /**
     * @brief Get number of active sessions
     * @return Count of active sessions
     */
    size_t getActiveSessionCount() const noexcept;

    /**
     * @brief Perform cleanup of closed sessions
     */
    void cleanupClosedSessions();

    /**
     * @brief Close all active sessions gracefully
     */
    void closeAllSessions();

    /**
     * @brief Check if session is managed by this manager
     * @param session Session to check
     * @return true if session is active
     */
    bool isSessionActive(std::shared_ptr<WebSocketSession> session) const;

    /**
     * @brief Set callback for session connection events
     * @param callback Function to call when sessions connect/disconnect
     */
    void setSessionCallback(SessionCallback callback);

    /**
     * @brief Get session statistics
     * @return Session count and health information
     */
    struct SessionStats {
        size_t total_sessions;
        size_t active_sessions;
        size_t closed_sessions;
    };
    SessionStats getSessionStats() const;

private:
    // Session management
    std::unordered_set<std::shared_ptr<WebSocketSession>> active_sessions_;
    mutable std::mutex sessions_mutex_;

    // Statistics tracking
    std::atomic<size_t> total_sessions_created_;
    std::atomic<size_t> total_sessions_closed_;

    // Callbacks
    SessionCallback session_callback_;

    /**
     * @brief Notify session callback about connection event
     * @param endpoint Client endpoint string
     * @param connected true if connected, false if disconnected
     */
    void notifySessionCallback(const std::string& endpoint, bool connected);
};

} // namespace siren::websocket