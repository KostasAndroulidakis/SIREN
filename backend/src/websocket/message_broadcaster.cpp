/**
 * @file message_broadcaster.cpp
 * @brief Implementation of WebSocket message broadcaster - MISRA C++ compliant
 * @author SIREN Project
 * @date 2025
 *
 * Single Responsibility: Broadcast messages to multiple WebSocket sessions ONLY
 */

#include "websocket/message_broadcaster.hpp"
#include "websocket/server.hpp" // For WebSocketSession definition
#include "utils/json_serializer.hpp"
#include "utils/error_handler.hpp"
#include <iostream>

namespace siren::websocket {

MessageBroadcaster::MessageBroadcaster()
    : running_(false)
    , initialized_(false)
    , total_broadcasts_(0)
    , failed_broadcasts_(0)
    , broadcast_callback_(nullptr)
{
    std::cout << "[" << COMPONENT_NAME << "] Initializing message broadcaster" << std::endl;
}

MessageBroadcaster::~MessageBroadcaster() {
    if (running_.load()) {
        stop();
    }
}

bool MessageBroadcaster::initialize() {
    if (initialized_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Already initialized",
                                             data::ErrorSeverity::WARNING);
        return true;
    }

    try {
        // Reset statistics
        total_broadcasts_.store(0);
        failed_broadcasts_.store(0);

        initialized_.store(true);

        std::cout << "[" << COMPONENT_NAME << "] Initialized successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "initialization", e,
                                           data::ErrorSeverity::FATAL);
        return false;
    }
}

bool MessageBroadcaster::start() {
    if (!initialized_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Not initialized - cannot start",
                                             data::ErrorSeverity::ERROR);
        return false;
    }

    if (running_.load()) {
        utils::ErrorHandler::handleSystemError(COMPONENT_NAME,
                                             "Already running",
                                             data::ErrorSeverity::WARNING);
        return true;
    }

    try {
        running_.store(true);

        std::cout << "[" << COMPONENT_NAME << "] Started successfully" << std::endl;
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "start operation", e,
                                           data::ErrorSeverity::ERROR);
        return false;
    }
}

void MessageBroadcaster::stop() {
    if (!running_.load()) {
        return;
    }

    std::cout << "[" << COMPONENT_NAME << "] Stopping message broadcaster..." << std::endl;

    running_.store(false);

    std::cout << "[" << COMPONENT_NAME << "] Stopped (broadcasts: " 
              << total_broadcasts_.load() << ", failures: " 
              << failed_broadcasts_.load() << ")" << std::endl;
}

bool MessageBroadcaster::isRunning() const noexcept {
    return running_.load();
}

void MessageBroadcaster::broadcastSonarData(const data::SonarDataPoint& data,
                                                    const SessionContainer& sessions) {
    if (!running_.load()) {
        return; // Not running
    }

    try {
        // Serialize sonar data to JSON (SSOT for sonar serialization)
        const std::string message = utils::JsonSerializer::serialize(data);

        // Broadcast to all sessions
        broadcastMessage(message, sessions);

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "sonar data broadcast", e,
                                           data::ErrorSeverity::ERROR);
        updateBroadcastStats(false);
    }
}

void MessageBroadcaster::broadcastPerformanceMetrics(const data::PerformanceMetrics& metrics,
                                                             const SessionContainer& sessions) {
    if (!running_.load()) {
        return; // Not running
    }

    try {
        // Serialize performance metrics to JSON (SSOT for metrics serialization)
        const std::string message = utils::JsonSerializer::serialize(metrics);

        // Broadcast to all sessions
        broadcastMessage(message, sessions);

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, "performance metrics broadcast", e,
                                           data::ErrorSeverity::ERROR);
        updateBroadcastStats(false);
    }
}

void MessageBroadcaster::broadcastMessage(const std::string& message,
                                                  const SessionContainer& sessions) {
    if (!running_.load() || message.empty()) {
        return;
    }

    size_t sessions_reached = 0;
    size_t total_sessions = sessions.size();

    // Broadcast to each active session
    for (const auto& session : sessions) {
        if (session && session->isAlive()) {
            if (sendToSession(session, message)) {
                ++sessions_reached;
            }
        }
    }

    // Update statistics and notify completion
    const bool success = (sessions_reached > 0 || total_sessions == 0);
    updateBroadcastStats(success);
    notifyBroadcastComplete(sessions_reached);

    // Log broadcast results for debugging
    if (total_sessions > 0) {
        std::cout << "[" << COMPONENT_NAME << "] Broadcast to " << sessions_reached 
                  << "/" << total_sessions << " sessions" << std::endl;
    }
}

void MessageBroadcaster::setBroadcastCallback(BroadcastCallback callback) {
    broadcast_callback_ = std::move(callback);
}

uint64_t MessageBroadcaster::getTotalBroadcasts() const noexcept {
    return total_broadcasts_.load();
}

uint64_t MessageBroadcaster::getFailedBroadcasts() const noexcept {
    return failed_broadcasts_.load();
}

bool MessageBroadcaster::sendToSession(std::shared_ptr<WebSocketSession> session,
                                               const std::string& message) {
    if (!session || !session->isAlive()) {
        return false;
    }

    try {
        session->sendMessage(message);
        return true;

    } catch (const std::exception& e) {
        utils::ErrorHandler::handleException(COMPONENT_NAME, 
                                           "session message send for " + session->getClientEndpoint(), 
                                           e, data::ErrorSeverity::WARNING);
        return false;
    }
}

void MessageBroadcaster::notifyBroadcastComplete(size_t sessions_reached) {
    if (broadcast_callback_) {
        try {
            broadcast_callback_(sessions_reached);
        } catch (const std::exception& e) {
            utils::ErrorHandler::handleException(COMPONENT_NAME, "broadcast completion callback", e,
                                               data::ErrorSeverity::WARNING);
        }
    }
}

void MessageBroadcaster::updateBroadcastStats(bool success) {
    total_broadcasts_.fetch_add(1);
    
    if (!success) {
        failed_broadcasts_.fetch_add(1);
    }
}

} // namespace siren::websocket