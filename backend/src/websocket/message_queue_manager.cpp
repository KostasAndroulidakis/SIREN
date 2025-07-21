/**
 * @file message_queue_manager.cpp
 * @brief Implementation of message queue manager - MISRA C++ compliant
 * @author KostasAndroulidakis
 * @date 2025
 *
 * Single Responsibility: Message queue management with backpressure
 */

#include "websocket/message_queue_manager.hpp"
#include "utils/error_handler.hpp"
#include <iostream>

namespace siren::websocket {

// SSOT for queue manager constants (MISRA C++ Rule 5.0.1)
namespace {
    constexpr const char* COMPONENT_NAME = "MessageQueueManager";
    constexpr size_t MAX_MESSAGE_QUEUE_SIZE = 100;  // Maximum queued messages (hard limit)
    constexpr size_t MESSAGE_QUEUE_WARNING_SIZE = 80;  // Warning threshold (80% of max)
}

MessageQueueManager::MessageQueueManager(const std::string& client_endpoint,
                                        QueueFullCallback queue_full_callback)
    : queue_mutex_()
    , message_queue_()
    , client_endpoint_(client_endpoint)
    , queue_full_callback_(std::move(queue_full_callback))
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing queue manager for " << client_endpoint_ << std::endl;
}

bool MessageQueueManager::enqueueMessage(const std::string& message,
                                        const std::atomic<bool>& /* write_in_progress */) {
    if (message.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(queue_mutex_);

    // Check queue size for backpressure management
    const size_t current_queue_size = message_queue_.size();
    
    if (current_queue_size >= MAX_MESSAGE_QUEUE_SIZE) {
        // CRITICAL: Hard limit reached - trigger client disconnection
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
            "Message queue full for client " + client_endpoint_ + " - triggering disconnect",
            data::ErrorSeverity::ERROR);
        
        // Trigger client disconnection through callback
        if (queue_full_callback_) {
            queue_full_callback_();
        }
        return false;
        
    } else if (current_queue_size >= MESSAGE_QUEUE_WARNING_SIZE) {
        // WARNING: Approaching limit - log warning for monitoring
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
            "Message queue approaching limit for client " + client_endpoint_ + 
            " (" + std::to_string(current_queue_size) + "/" + std::to_string(MAX_MESSAGE_QUEUE_SIZE) + ")",
            data::ErrorSeverity::WARNING);
    }

    // Normal operation - enqueue message
    message_queue_.push(message);
    return true;
}

bool MessageQueueManager::getNextMessage(std::string& message) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    if (message_queue_.empty()) {
        return false;
    }

    message = message_queue_.front();
    message_queue_.pop();
    return true;
}

bool MessageQueueManager::isEmpty() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return message_queue_.empty();
}

size_t MessageQueueManager::size() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return message_queue_.size();
}

void MessageQueueManager::clear() {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    
    // Clear queue by swapping with empty queue (efficient)
    std::queue<std::string> empty_queue;
    message_queue_.swap(empty_queue);
    
    std::cout << "[" << COMPONENT_NAME << "] Cleared message queue for " << client_endpoint_ << std::endl;
}

} // namespace siren::websocket