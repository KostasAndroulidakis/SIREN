/**
 * @file websocket_message_broadcaster.hpp
 * @brief WebSocket message broadcaster - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Broadcast messages to multiple WebSocket sessions ONLY
 * MISRA C++ 2023 compliant - Rule 5.0.1 (no magic numbers), Rule 21.2.1 (RAII)
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <atomic>
#include "data/sonar_types.hpp"

namespace siren::websocket {

// Forward declarations
class WebSocketSession;

/**
 * @brief WebSocket message broadcaster with single responsibility
 *
 * RESPONSIBILITIES:
 * - Broadcast sonar data to multiple sessions ONLY
 * - Broadcast performance metrics to multiple sessions ONLY
 * - Handle broadcast completion tracking ONLY
 * - Manage broadcast error handling ONLY
 *
 * NOT RESPONSIBLE FOR:
 * - Session lifecycle management (handled by SessionManager)
 * - TCP connection acceptance (handled by ConnectionAcceptor)
 * - Statistics collection (handled by StatisticsCollector)
 * - Server coordination (handled by WebSocketServer)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */
class WebSocketMessageBroadcaster {
public:
    /// Session container type (SSOT for session handling)
    using SessionContainer = std::vector<std::shared_ptr<WebSocketSession>>;
    
    /// Callback type for broadcast completion (SSOT)
    using BroadcastCallback = std::function<void(size_t)>;

    /**
     * @brief Constructor - RAII initialization
     */
    explicit WebSocketMessageBroadcaster();

    /**
     * @brief Destructor - RAII cleanup
     */
    ~WebSocketMessageBroadcaster();

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    WebSocketMessageBroadcaster(const WebSocketMessageBroadcaster&) = delete;
    WebSocketMessageBroadcaster& operator=(const WebSocketMessageBroadcaster&) = delete;
    WebSocketMessageBroadcaster(WebSocketMessageBroadcaster&&) = delete;
    WebSocketMessageBroadcaster& operator=(WebSocketMessageBroadcaster&&) = delete;

    /**
     * @brief Initialize the message broadcaster
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start the message broadcaster
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop the message broadcaster
     */
    void stop();

    /**
     * @brief Check if broadcaster is running
     */
    bool isRunning() const noexcept;

    /**
     * @brief Broadcast sonar data to all active sessions (SSOT for sonar broadcasting)
     * @param data Sonar data point to broadcast
     * @param sessions Container of active sessions to broadcast to
     */
    void broadcastSonarData(const data::SonarDataPoint& data, 
                           const SessionContainer& sessions);

    /**
     * @brief Broadcast performance metrics to all active sessions (SSOT for metrics broadcasting)
     * @param metrics Performance metrics to broadcast
     * @param sessions Container of active sessions to broadcast to
     */
    void broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics, 
                                   const SessionContainer& sessions);

    /**
     * @brief Broadcast generic message to all active sessions (SSOT for message broadcasting)
     * @param message Serialized message to broadcast
     * @param sessions Container of active sessions to broadcast to
     */
    void broadcastMessage(const std::string& message, 
                         const SessionContainer& sessions);

    /**
     * @brief Set callback for broadcast completion (SSOT for callback setting)
     * @param callback Function to call when broadcast completes
     */
    void setBroadcastCallback(BroadcastCallback callback);

    /**
     * @brief Get total number of successful broadcasts (SSOT for broadcast counting)
     */
    uint64_t getTotalBroadcasts() const noexcept;

    /**
     * @brief Get total number of failed broadcasts (SSOT for error counting)
     */
    uint64_t getFailedBroadcasts() const noexcept;

private:
    // State management
    std::atomic<bool> running_;
    std::atomic<bool> initialized_;

    // Broadcasting statistics (thread-safe)
    std::atomic<uint64_t> total_broadcasts_;
    std::atomic<uint64_t> failed_broadcasts_;

    // Broadcast completion notification (SSOT)
    BroadcastCallback broadcast_callback_;

    // SSOT constants (MISRA C++ Rule 5.0.1)
    static constexpr const char* COMPONENT_NAME = "MessageBroadcaster";

    /**
     * @brief Send message to individual session (SSOT for session messaging)
     * @param session Target session
     * @param message Message to send
     * @return true if message sent successfully
     */
    bool sendToSession(std::shared_ptr<WebSocketSession> session, 
                      const std::string& message);

    /**
     * @brief Notify broadcast completion (SSOT for completion notification)
     * @param sessions_reached Number of sessions that received the message
     */
    void notifyBroadcastComplete(size_t sessions_reached);

    /**
     * @brief Update broadcast statistics (SSOT for statistics tracking)
     * @param success true if broadcast was successful
     */
    void updateBroadcastStats(bool success);
};

} // namespace siren::websocket