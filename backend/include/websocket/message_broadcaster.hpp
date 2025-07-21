/**
 * @file message_broadcaster.hpp
 * @brief Military-grade WebSocket message broadcaster for data distribution
 * @author SIREN Project
 * @date 2025
 *
 * Specialized broadcaster class responsible for distributing messages to
 * multiple WebSocket sessions. Follows SRP by focusing exclusively on
 * message broadcasting and distribution coordination.
 */

#pragma once

#include <memory>
#include <unordered_set>
#include <functional>
#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include "data/sonar_types.hpp"

namespace siren::websocket {

// Forward declarations
class WebSocketSession;
class WebSocketSessionManager;

/**
 * @brief Military-grade WebSocket message broadcaster
 *
 * Handles message distribution to multiple WebSocket sessions with
 * thread-safe operations and performance optimization. Follows SRP
 * by focusing exclusively on message broadcasting coordination.
 *
 * Features:
 * - Thread-safe message broadcasting
 * - Asynchronous message distribution
 * - Message queue management
 * - Performance optimization for high-frequency data
 * - Military-grade error handling
 * - Message serialization coordination
 */
class WebSocketMessageBroadcaster {
public:
    /// Message broadcast callback type
    using BroadcastCallback = std::function<void(size_t)>;

    /**
     * @brief Constructor
     */
    explicit WebSocketMessageBroadcaster();

    /**
     * @brief Destructor - ensures clean shutdown
     */
    ~WebSocketMessageBroadcaster();

    // Non-copyable, non-movable
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
     * @brief Check if broadcaster is active
     */
    bool isActive() const noexcept;

    /**
     * @brief Broadcast sonar data to all active sessions
     * @param data Sonar data point to broadcast
     * @param sessions Active WebSocket sessions to broadcast to
     */
    void broadcastSonarData(const data::SonarDataPoint& data,
                           const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions);

    /**
     * @brief Broadcast performance metrics to all active sessions
     * @param metrics Performance metrics to broadcast
     * @param sessions Active WebSocket sessions to broadcast to
     */
    void broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics,
                                   const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions);

    /**
     * @brief Broadcast raw message to all active sessions
     * @param message Raw message string to broadcast
     * @param sessions Active WebSocket sessions to broadcast to
     */
    void broadcastRawMessage(const std::string& message,
                           const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions);

    /**
     * @brief Get broadcast statistics
     */
    struct BroadcastStats {
        uint64_t total_messages_sent;
        uint64_t total_sessions_reached;
        uint64_t failed_broadcasts;
        uint32_t average_broadcast_time_us;
    };
    BroadcastStats getBroadcastStats() const;

    /**
     * @brief Set callback for broadcast events
     * @param callback Function to call after each broadcast
     */
    void setBroadcastCallback(BroadcastCallback callback);

private:
    /// Message types for internal queue
    enum class MessageType {
        SONAR_DATA,
        PERFORMANCE_METRICS,
        RAW_MESSAGE
    };

    /// Internal message structure
    struct BroadcastMessage {
        MessageType type;
        std::string serialized_data;
        std::unordered_set<std::shared_ptr<WebSocketSession>> target_sessions;
        std::chrono::steady_clock::time_point timestamp;
    };

    // Core state
    std::atomic<bool> active_;
    std::atomic<bool> shutdown_requested_;

    // Message queue and processing
    std::queue<BroadcastMessage> message_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::thread broadcaster_thread_;

    // Statistics tracking
    std::atomic<uint64_t> total_messages_sent_;
    std::atomic<uint64_t> total_sessions_reached_;
    std::atomic<uint64_t> failed_broadcasts_;
    std::atomic<uint32_t> average_broadcast_time_us_;

    // Callbacks
    BroadcastCallback broadcast_callback_;

    /**
     * @brief Main broadcasting thread function
     */
    void broadcasterThreadFunction();

    /**
     * @brief Process a single broadcast message
     * @param message Message to broadcast
     */
    void processBroadcastMessage(const BroadcastMessage& message);

    /**
     * @brief Send message to a specific session
     * @param session Target WebSocket session
     * @param message Message to send
     * @return true if successful
     */
    bool sendMessageToSession(std::shared_ptr<WebSocketSession> session, const std::string& message);

    /**
     * @brief Serialize sonar data to JSON
     * @param data Sonar data point to serialize
     * @return JSON string representation
     */
    std::string serializeSonarData(const data::SonarDataPoint& data);

    /**
     * @brief Serialize performance metrics to JSON
     * @param metrics Performance metrics to serialize
     * @return JSON string representation
     */
    std::string serializePerformanceMetrics(const data::PerformanceMetrics& metrics);

    /**
     * @brief Enqueue message for broadcasting
     * @param message Message to enqueue
     */
    void enqueueMessage(BroadcastMessage message);

    /**
     * @brief Update broadcast statistics
     * @param sessions_reached Number of sessions reached
     * @param broadcast_time_us Time taken for broadcast in microseconds
     */
    void updateBroadcastStats(size_t sessions_reached, uint32_t broadcast_time_us);
};

} // namespace siren::websocket