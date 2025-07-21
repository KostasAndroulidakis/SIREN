/**
 * @file message_queue_manager.hpp
 * @brief Message queue management with backpressure - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Message queue management with backpressure for one client ONLY
 *
 * RESPONSIBILITIES:
 * - Thread-safe message queuing for single client
 * - Queue size monitoring and backpressure implementation
 * - Client disconnect decisions based on queue overflow
 * - Queue state management (empty/full checks)
 *
 * NOT RESPONSIBLE FOR:
 * - WebSocket protocol handling (handled by WebSocketSession)
 * - Message serialization (handled by caller)
 * - Network I/O operations (handled by WebSocketSession)
 * - Session lifecycle management (handled by SessionManager)
 *
 * MISRA C++ Compliance:
 * - Rule 5.0.1: All constants defined, no magic numbers
 * - Rule 21.2.1: RAII for all resources
 * - Rule 8.4.1: Single responsibility per class
 */

#pragma once

#include <mutex>
#include <queue>
#include <string>
#include <atomic>
#include <functional>

namespace siren::websocket {

/**
 * @brief Message queue manager with single responsibility
 *
 * Handles message queuing with backpressure management for a single client
 * without mixing concerns with WebSocket protocol or network operations.
 *
 * Single Responsibility: Message queue management with backpressure
 */
class MessageQueueManager {
public:
    /// Queue full callback type - called when client should be disconnected
    using QueueFullCallback = std::function<void()>;

    /**
     * @brief Constructor - RAII initialization
     * @param client_endpoint Client endpoint string for logging
     * @param queue_full_callback Callback when queue is full (client disconnect)
     */
    explicit MessageQueueManager(const std::string& client_endpoint,
                                QueueFullCallback queue_full_callback);

    /**
     * @brief Destructor - RAII cleanup
     */
    ~MessageQueueManager() = default;

    // MISRA C++ Rule 12.1.1: Disable copy/move for resource management
    MessageQueueManager(const MessageQueueManager&) = delete;
    MessageQueueManager& operator=(const MessageQueueManager&) = delete;
    MessageQueueManager(MessageQueueManager&&) = delete;
    MessageQueueManager& operator=(MessageQueueManager&&) = delete;

    /**
     * @brief Enqueue message with backpressure management (SSOT for queuing)
     * @param message Message to enqueue
     * @param write_in_progress Current write state
     * @return true if message enqueued, false if client should be disconnected
     */
    bool enqueueMessage(const std::string& message, 
                       const std::atomic<bool>& write_in_progress);

    /**
     * @brief Get next message from queue (SSOT for dequeuing)
     * @param message Output parameter for the message
     * @return true if message retrieved, false if queue empty
     */
    bool getNextMessage(std::string& message);

    /**
     * @brief Check if queue is empty (SSOT for queue state)
     * @return true if queue is empty
     */
    bool isEmpty() const;

    /**
     * @brief Get current queue size (SSOT for queue metrics)
     * @return Current number of queued messages
     */
    size_t size() const;

    /**
     * @brief Clear all messages from queue (SSOT for queue cleanup)
     */
    void clear();

private:
    // Queue state management
    mutable std::mutex queue_mutex_;
    std::queue<std::string> message_queue_;

    // Client information
    std::string client_endpoint_;

    // Callbacks
    QueueFullCallback queue_full_callback_;
};

} // namespace siren::websocket