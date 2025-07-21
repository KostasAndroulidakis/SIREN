/**
 * @file message_broadcaster.cpp
 * @brief Implementation of military-grade WebSocket message broadcaster
 * @author SIREN Project
 * @date 2025
 *
 * Specialized broadcaster for WebSocket message distribution.
 * Follows SRP by focusing exclusively on message broadcasting
 * and distribution coordination.
 */

#include "websocket/message_broadcaster.hpp"
#include "websocket/server.hpp"
#include "utils/json_serializer.hpp"
#include <iostream>
#include <chrono>

namespace siren::websocket {

WebSocketMessageBroadcaster::WebSocketMessageBroadcaster()
    : active_(false)
    , shutdown_requested_(false)
    , total_messages_sent_(0)
    , total_sessions_reached_(0)
    , failed_broadcasts_(0)
    , average_broadcast_time_us_(0)
{
    std::cout << "[WebSocketMessageBroadcaster] Initializing message broadcaster" << std::endl;
}

WebSocketMessageBroadcaster::~WebSocketMessageBroadcaster() {
    stop();
    std::cout << "[WebSocketMessageBroadcaster] Message broadcaster destroyed" << std::endl;
}

bool WebSocketMessageBroadcaster::initialize() {
    std::cout << "[WebSocketMessageBroadcaster] ✅ Message broadcaster initialized" << std::endl;
    return true;
}

bool WebSocketMessageBroadcaster::start() {
    if (active_.load()) {
        std::cout << "[WebSocketMessageBroadcaster] ⚠️ Message broadcaster already active" << std::endl;
        return true;
    }

    try {
        active_.store(true);
        shutdown_requested_.store(false);

        // Start broadcaster thread
        broadcaster_thread_ = std::thread(&WebSocketMessageBroadcaster::broadcasterThreadFunction, this);

        std::cout << "[WebSocketMessageBroadcaster] 🚀 Message broadcaster started" << std::endl;
        return true;

    } catch (const std::exception& e) {
        std::cout << "[WebSocketMessageBroadcaster] ❌ Failed to start broadcaster: " << e.what() << std::endl;
        active_.store(false);
        return false;
    }
}

void WebSocketMessageBroadcaster::stop() {
    if (!active_.load()) {
        return;
    }

    std::cout << "[WebSocketMessageBroadcaster] 🛑 Stopping message broadcaster..." << std::endl;

    shutdown_requested_.store(true);
    active_.store(false);

    // Wake up broadcaster thread
    queue_cv_.notify_all();

    // Join broadcaster thread
    if (broadcaster_thread_.joinable()) {
        broadcaster_thread_.join();
    }

    // Clear remaining messages
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        std::queue<BroadcastMessage> empty;
        message_queue_.swap(empty);
    }

    std::cout << "[WebSocketMessageBroadcaster] ✅ Message broadcaster stopped" << std::endl;
}

bool WebSocketMessageBroadcaster::isActive() const noexcept {
    return active_.load();
}

void WebSocketMessageBroadcaster::broadcastSonarData(const data::SonarDataPoint& data,
                                                    const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions) {
    if (!active_.load()) {
        return;
    }

    BroadcastMessage message;
    message.type = MessageType::SONAR_DATA;
    message.serialized_data = serializeSonarData(data);
    message.target_sessions = sessions;
    message.timestamp = std::chrono::steady_clock::now();

    enqueueMessage(std::move(message));
}

void WebSocketMessageBroadcaster::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics,
                                                             const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions) {
    if (!active_.load()) {
        return;
    }

    BroadcastMessage message;
    message.type = MessageType::PERFORMANCE_METRICS;
    message.serialized_data = serializePerformanceMetrics(metrics);
    message.target_sessions = sessions;
    message.timestamp = std::chrono::steady_clock::now();

    enqueueMessage(std::move(message));
}

void WebSocketMessageBroadcaster::broadcastRawMessage(const std::string& message,
                                                     const std::unordered_set<std::shared_ptr<WebSocketSession>>& sessions) {
    if (!active_.load()) {
        return;
    }

    BroadcastMessage broadcast_msg;
    broadcast_msg.type = MessageType::RAW_MESSAGE;
    broadcast_msg.serialized_data = message;
    broadcast_msg.target_sessions = sessions;
    broadcast_msg.timestamp = std::chrono::steady_clock::now();

    enqueueMessage(std::move(broadcast_msg));
}

WebSocketMessageBroadcaster::BroadcastStats WebSocketMessageBroadcaster::getBroadcastStats() const {
    return BroadcastStats{
        .total_messages_sent = total_messages_sent_.load(),
        .total_sessions_reached = total_sessions_reached_.load(),
        .failed_broadcasts = failed_broadcasts_.load(),
        .average_broadcast_time_us = average_broadcast_time_us_.load()
    };
}

void WebSocketMessageBroadcaster::setBroadcastCallback(BroadcastCallback callback) {
    broadcast_callback_ = std::move(callback);
}

void WebSocketMessageBroadcaster::broadcasterThreadFunction() {
    std::cout << "[WebSocketMessageBroadcaster] 🧵 Broadcaster thread started" << std::endl;

    while (!shutdown_requested_.load()) {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        // Wait for messages or shutdown
        queue_cv_.wait(lock, [this] {
            return !message_queue_.empty() || shutdown_requested_.load();
        });

        if (shutdown_requested_.load()) {
            break;
        }

        // Process all available messages
        while (!message_queue_.empty()) {
            BroadcastMessage message = std::move(message_queue_.front());
            message_queue_.pop();
            lock.unlock();

            // Process the message
            processBroadcastMessage(message);

            lock.lock();
        }
    }

    std::cout << "[WebSocketMessageBroadcaster] 🧵 Broadcaster thread stopped" << std::endl;
}

void WebSocketMessageBroadcaster::processBroadcastMessage(const BroadcastMessage& message) {
    auto start_time = std::chrono::steady_clock::now();
    size_t sessions_reached = 0;
    size_t failed_sends = 0;

    for (auto& session : message.target_sessions) {
        if (session && session->isAlive()) {
            if (sendMessageToSession(session, message.serialized_data)) {
                sessions_reached++;
            } else {
                failed_sends++;
            }
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    auto broadcast_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    // Update statistics
    updateBroadcastStats(sessions_reached, static_cast<uint32_t>(broadcast_time_us));

    if (failed_sends > 0) {
        failed_broadcasts_.fetch_add(failed_sends);
    }

    // Notify callback
    if (broadcast_callback_) {
        broadcast_callback_(sessions_reached);
    }
}

bool WebSocketMessageBroadcaster::sendMessageToSession(std::shared_ptr<WebSocketSession> session, const std::string& message) {
    try {
        // MISRA C++ compliance: Use correct public interface method
        // Send the serialized message through the public interface
        session->sendMessage(message);
        return true;

    } catch (const std::exception& e) {
        std::cout << "[WebSocketMessageBroadcaster] ❌ Failed to send message to session: " << e.what() << std::endl;
        return false;
    }
}

std::string WebSocketMessageBroadcaster::serializeSonarData(const data::SonarDataPoint& data) {
    return utils::JsonSerializer::serialize(data);
}

std::string WebSocketMessageBroadcaster::serializePerformanceMetrics(const data::PerformanceMetrics& metrics) {
    return utils::JsonSerializer::serialize(metrics);
}

void WebSocketMessageBroadcaster::enqueueMessage(BroadcastMessage message) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.push(std::move(message));
    }

    // Notify broadcaster thread
    queue_cv_.notify_one();
}

void WebSocketMessageBroadcaster::updateBroadcastStats(size_t sessions_reached, uint32_t broadcast_time_us) {
    total_messages_sent_.fetch_add(1);
    total_sessions_reached_.fetch_add(sessions_reached);

    // Update average broadcast time (simplified moving average)
    uint32_t current_avg = average_broadcast_time_us_.load();
    uint32_t new_avg = (current_avg + broadcast_time_us) / 2;
    average_broadcast_time_us_.store(new_avg);
}

} // namespace siren::websocket